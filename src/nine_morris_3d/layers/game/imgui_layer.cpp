#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include "application/events.h"
#include "application/platform.h"
#include "application/input.h"
#include "graphics/debug_opengl.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/save_load.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/assets.h"
#include "other/paths.h"
#include "other/logging.h"
#include "other/assert.h"
#include "other/user_data.h"

#define RESET_HOVERING_GUI() hovering_gui = false
#define HOVERING_GUI() hovering_gui = true

#define DEFAULT_BROWN ImVec4(0.647f, 0.4f, 0.212f, 1.0f)
#define DARK_BROWN ImVec4(0.4f, 0.25f, 0.1f, 1.0f)
#define LIGHT_BROWN ImVec4(0.68f, 0.48f, 0.22f, 1.0f)
#define BEIGE ImVec4(0.961f, 0.875f, 0.733f, 1.0f)
#define LIGHT_GRAY_BLUE ImVec4(0.357f, 0.408f, 0.525f, 1.0f)

void ImGuiLayer::on_attach() {
    save_load::GameState state;
    try {
        save_load::load_game_from_file(state);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARN("{}", e.what());
        save_load::handle_save_file_not_open_error();
        REL_WARN("Could not load game");
    } catch (const save_load::SaveFileError& e) {
        REL_WARN("{}", e.what());  // TODO maybe delete file
        REL_WARN("Could not load game");
    }
    last_save_game_date = std::move(state.date);
    DEB_INFO("Checked last saved game");

    try {
        info_file_path = paths::path_for_logs(INFO_FILE);
        save_game_file_path = paths::path_for_saved_data(save_load::SAVE_GAME_FILE);
    } catch (const user_data::UserNameError& e) {
        REL_ERROR("{}", e.what());

        info_file_path = INFO_FILE;
        save_game_file_path = save_load::SAVE_GAME_FILE;
    }
}

void ImGuiLayer::on_detach() {
    // These need to be resetted
    hovering_gui = false;
    can_undo = false;
    can_redo = false;
    show_info = false;
    show_about = false;
    show_could_not_load_game = false;
    show_no_last_game = false;
}

void ImGuiLayer::on_awake() {
    game_layer = get_layer<GameLayer>("game");
    gui_layer = get_layer<GuiLayer>("gui");

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigWindowsResizeFromEdges = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
#ifdef PLATFORM_GAME_RELEASE
    io.IniFilename = nullptr;
#endif

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = DEFAULT_BROWN;
    colors[ImGuiCol_TitleBgActive] = DEFAULT_BROWN;
    colors[ImGuiCol_FrameBg] = DEFAULT_BROWN;
    colors[ImGuiCol_FrameBgHovered] = DARK_BROWN;
    colors[ImGuiCol_FrameBgActive] = LIGHT_BROWN;
    colors[ImGuiCol_Button] = DARK_BROWN;
    colors[ImGuiCol_ButtonHovered] = DEFAULT_BROWN;
    colors[ImGuiCol_ButtonActive] = LIGHT_BROWN;
    colors[ImGuiCol_Header] = DARK_BROWN;
    colors[ImGuiCol_HeaderHovered] = DEFAULT_BROWN;
    colors[ImGuiCol_HeaderActive] = LIGHT_BROWN;
    colors[ImGuiCol_CheckMark] = BEIGE;
    colors[ImGuiCol_SliderGrab] = LIGHT_GRAY_BLUE;
    colors[ImGuiCol_SliderGrabActive] = LIGHT_GRAY_BLUE;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.FrameRounding = 8;
    style.WindowRounding = 8;
    style.ChildRounding = 8;
    style.PopupRounding = 8;
    style.GrabRounding = 8;

    // It's ok to be called multiple times
    LOG_TOTAL_GPU_MEMORY_ALLOCATED();
}

void ImGuiLayer::on_update(float dt) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(app->app_data.width, app->app_data.height);
    io.DeltaTime = dt;

    RESET_HOVERING_GUI();

    if (!show_about && ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game", nullptr, false)) {
                app->change_scene("game");

                DEB_INFO("Restarting game");
            }
            if (ImGui::MenuItem("Load Last", nullptr, false)) {
                game_layer->board.finalize_pieces_state();

                game_layer->load_game();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", last_save_game_date.c_str());
            }
            if (ImGui::MenuItem("Save", nullptr, false)) {
                game_layer->board.finalize_pieces_state();

                save_load::GameState state;
                state.board = game_layer->board;
                state.camera = app->camera;
                state.time = gui_layer->timer.get_time_raw();

                time_t current;
                time(&current);
                state.date = ctime(&current);

                state.state_history = {
                    game_layer->state_history.undo_state_history,
                    game_layer->state_history.redo_state_history
                };

                try {
                    save_load::save_game_to_file(state);
                } catch (const save_load::SaveFileNotOpenError& e) {
                    REL_ERROR("{}", e.what());
                    save_load::handle_save_file_not_open_error();
                    REL_ERROR("Could not save game");
                } catch (const save_load::SaveFileError& e) {
                    REL_ERROR("{}", e.what());
                    REL_ERROR("Could not save game");
                }

                last_save_game_date = std::move(state.date);
            }
            if (ImGui::MenuItem("Undo", nullptr, false, can_undo)) {
                const bool undid_game_over = game_layer->board.undo();

                if (game_layer->board.undo_state_history->empty()) {
                    can_undo = false;
                }

                if (undid_game_over) {
                    gui_layer->timer.start(app->window->get_time());
                }
            }
            if (ImGui::MenuItem("Redo", nullptr, false, can_redo)) {
                const bool redid_game_over = game_layer->board.redo();

                if (game_layer->board.redo_state_history->empty()) {
                    can_redo = false;
                }

                if (redid_game_over) {
                    gui_layer->timer.stop();
                    game_layer->board.phase = Board::Phase::GameOver;
                }
            }
            if (ImGui::MenuItem("Exit", nullptr, false)) {
                app->running = false;
            }

            ImGui::EndMenu();
            HOVERING_GUI();
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::BeginMenu("Graphics")) {
                if (ImGui::MenuItem("VSync", nullptr, &app->options.vsync)) {
                    if (app->options.vsync) {
                        app->window->set_vsync(app->options.vsync);

                        DEB_INFO("VSync enabled");
                    } else {
                        app->window->set_vsync(app->options.vsync);

                        DEB_INFO("VSync disabled");
                    }
                }
                if (ImGui::BeginMenu("Anti-Aliasing")) {
                    if (ImGui::RadioButton("Off", &app->options.samples, 1)) {
                        game_layer->set_scene_framebuffer(app->options.samples);

                        DEB_INFO("Anti-aliasing disabled");
                    }
                    if (ImGui::RadioButton("2x", &app->options.samples, 2)) {
                        game_layer->set_scene_framebuffer(app->options.samples);

                        DEB_INFO("2x anti-aliasing");
                    }
                    if (ImGui::RadioButton("4x", &app->options.samples, 4)) {
                        game_layer->set_scene_framebuffer(app->options.samples);

                        DEB_INFO("4x anti-aliasing");
                    }

                    ImGui::EndMenu();
                    HOVERING_GUI();
                }
                if (ImGui::BeginMenu("Texture Quality")) {
                    static int dummy = get_texture_quality_option(app->options.texture_quality);

                    if (ImGui::RadioButton("Low", &dummy, 0)) {
                        game_layer->set_texture_quality(options::LOW);
                        app->renderer->set_depth_map_framebuffer(2048);

                        DEB_INFO("Textures set to {} quality", options::LOW);
                    }
                    if (ImGui::RadioButton("Normal", &dummy, 1)) {
                        game_layer->set_texture_quality(options::NORMAL);
                        app->renderer->set_depth_map_framebuffer(4096);

                        DEB_INFO("Textures set to {} quality", options::NORMAL);
                    }

                    ImGui::EndMenu();
                    HOVERING_GUI();
                }
                if (ImGui::BeginMenu("Anisotropic Filtering")) {
                    if (ImGui::RadioButton("Off", &app->options.anisotropic_filtering, 0)) {
                        game_layer->resetup_textures();

                        DEB_INFO("Anisotropic filtering disabled");
                    }
                    if (ImGui::RadioButton("4x", &app->options.anisotropic_filtering, 4)) {
                        game_layer->resetup_textures();

                        DEB_INFO("4x Anisotropic filtering");
                    }
                    if (ImGui::RadioButton("8x", &app->options.anisotropic_filtering, 8)) {
                        game_layer->resetup_textures();

                        DEB_INFO("8x Anisotropic filtering");
                    }

                    ImGui::EndMenu();
                    HOVERING_GUI();
                }
                if (ImGui::MenuItem("Custom Cursor", nullptr, &app->options.custom_cursor)) {
                    if (app->options.custom_cursor) {
                        if (game_layer->board.should_take_piece) {
                            app->window->set_cursor(app->cross_cursor);
                        } else {
                            app->window->set_cursor(app->arrow_cursor);
                        }

                        DEB_INFO("Set custom cursor");
                    } else {
                        app->window->set_cursor(0);

                        DEB_INFO("Set default cursor");
                    }
                }
                static bool normal_mapping = app->options.normal_mapping;
                if (ImGui::MenuItem("Normal Mapping", nullptr, &normal_mapping)) {
                    if (normal_mapping) {
                        game_layer->set_normal_mapping(normal_mapping);

                        DEB_INFO("Normal mapping enabled");
                    } else {
                        game_layer->set_normal_mapping(normal_mapping);

                        DEB_INFO("Normal mapping disabled");
                    }
                }
                if (ImGui::MenuItem("Bloom", nullptr, &app->options.bloom)) {
                    if (app->options.bloom) {
                        app->set_bloom(app->options.bloom);

                        DEB_INFO("Bloom enabled");
                    } else {
                        app->set_bloom(app->options.bloom);

                        DEB_INFO("Bloom disabled");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI();
            }
            if (ImGui::MenuItem("Save On Exit", nullptr, &app->options.save_on_exit)) {
                if (app->options.save_on_exit) {
                    DEB_INFO("The game will be saved on exit");
                } else {
                    DEB_INFO("The game will not be saved on exit");
                }
            }
            if (ImGui::BeginMenu("Skybox")) {
                static int dummy = get_skybox_option(app->options.skybox);

                if (ImGui::RadioButton("Field", &dummy, 0)) {
                    game_layer->set_skybox(options::FIELD);

                    DEB_INFO("Skybox set to {}", options::FIELD);
                }
                if (ImGui::RadioButton("Autumn", &dummy, 1)) {
                    game_layer->set_skybox(options::AUTUMN);

                    DEB_INFO("Skybox set to {}", options::AUTUMN);
                }

                ImGui::EndMenu();
                HOVERING_GUI();
            }
            if (ImGui::MenuItem("Show Info", nullptr, &show_info)) {
                if (show_info) {
                    DEB_INFO("Show info");
                } else {
                    DEB_INFO("Hide info");
                }
            }
            if (ImGui::BeginMenu("Camera Sensitivity")) {
                ImGui::PushItemWidth(100.0f);
                if (ImGui::SliderFloat("##", &app->options.sensitivity, 0.5f, 2.0f, "%.01f", ImGuiSliderFlags_Logarithmic)) {
                    app->camera.sensitivity = app->options.sensitivity;

                    DEB_INFO("Changed camera sensitivity to {}", app->camera.sensitivity);
                }
                ImGui::PopItemWidth();

                ImGui::EndMenu();
                HOVERING_GUI();
            }
            if (ImGui::BeginMenu("User Interface")) {
                if (ImGui::MenuItem("Hide Timer", nullptr, &app->options.hide_timer)) {
                    if (app->options.hide_timer) {
                        app->gui_renderer->remove_widget(gui_layer->timer_text);

                        DEB_INFO("Hide timer");
                    } else {
                        app->gui_renderer->add_widget(gui_layer->timer_text);

                        DEB_INFO("Show timer");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI();
            }
            static bool labeled_board = app->options.labeled_board;

            if (ImGui::MenuItem("Labeled Board", nullptr, &labeled_board)) {
                if (labeled_board) {
                    game_layer->set_labeled_board_texture(labeled_board);

                    DEB_INFO("Labeled board");
                } else {
                    game_layer->set_labeled_board_texture(labeled_board);

                    DEB_INFO("Non-labeled board");
                }
            }

            ImGui::EndMenu();
            HOVERING_GUI();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About", nullptr, false)) {
                show_about = true;
            }
            if (ImGui::MenuItem("Log Info", nullptr, false)) {
                logging::log_opengl_and_dependencies_info(logging::LogTarget::File, INFO_FILE);

                DEB_INFO("Logged OpenGL and dependencies info");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", info_file_path.c_str());
            }

            ImGui::EndMenu();
            HOVERING_GUI();
        }

        ImGui::EndMainMenuBar();
    }

    if (show_about) {
        draw_about();
    } else if (show_could_not_load_game) {
        draw_could_not_load_game();
    } else if (show_no_last_game) {
        draw_no_last_game();
    } else if (game_layer->board.phase == Board::Phase::GameOver) {
        draw_game_over();
    }

    if (game_layer->board.undo_state_history->size() > 0) {
        can_undo = true;
    }

    if (game_layer->board.redo_state_history->size() > 0) {
        can_redo = true;
    }

    if (show_info && !show_about) {
        ImGui::PushFont(app->data.imgui_info_font);
        ImGui::Begin("Info", nullptr, ImGuiWindowFlags_NoCollapse);
        ImGui::Text("FPS: %f", app->fps);
        ImGui::Text("OpenGL: %s", debug_opengl::get_opengl_version());
        ImGui::Text("GLSL: %s", debug_opengl::get_glsl_version());
        ImGui::Text("Vendor: %s", debug_opengl::get_vendor());
        ImGui::Text("Renderer: %s", debug_opengl::get_renderer());
        ImGui::End();
        ImGui::PopFont();
    }

#ifdef PLATFORM_GAME_DEBUG
    draw_debug(dt);
#endif
}

void ImGuiLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher (event);

    dispatcher.dispatch<MouseScrolledEvent>(MouseScrolled, BIND(ImGuiLayer::on_mouse_scrolled));
    dispatcher.dispatch<MouseMovedEvent>(MouseMoved, BIND(ImGuiLayer::on_mouse_moved));
    dispatcher.dispatch<MouseButtonPressedEvent>(MouseButtonPressed, BIND(ImGuiLayer::on_mouse_button_pressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(MouseButtonReleased, BIND(ImGuiLayer::on_mouse_button_released));
    dispatcher.dispatch<KeyPressedEvent>(KeyPressed, BIND(ImGuiLayer::on_key_pressed));
}

bool ImGuiLayer::on_mouse_scrolled(events::MouseScrolledEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel = event.scroll;

    return hovering_gui;
}

bool ImGuiLayer::on_mouse_moved(events::MouseMovedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MousePos = ImVec2(event.mouse_x, event.mouse_y);

    return false;
}

bool ImGuiLayer::on_mouse_button_pressed(events::MouseButtonPressedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[static_cast<int>(event.button)] = true;

    return hovering_gui;
}

bool ImGuiLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[static_cast<int>(event.button)] = false;

    return hovering_gui;
}

// TODO think about to include this or not
bool ImGuiLayer::on_key_pressed(events::KeyPressedEvent& event) {
    if (event.repeat) {
        return false;
    }

    if (event.control) {
        switch (event.key) {
            case input::Key::Z:
                break;
            case input::Key::Y:
                break;
            default:
                break;
        }
    }

    return false;
}

void ImGuiLayer::draw_game_over() {
    ImGui::PushFont(app->data.imgui_windows_font);
    ImGui::OpenPopup("Game Over");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Game Over", nullptr, ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::Dummy(ImVec2(0.0f, 4.0f));

        switch (game_layer->board.ending) {
            case Board::Ending::WinnerWhite: {
                const char* message1 = "White player wins!";
                draw_game_over_message(message1, game_layer->board.get_ending_message());
                break;
            }
            case Board::Ending::WinnerBlack: {
                const char* message1 = "Black player wins!";
                draw_game_over_message(message1, game_layer->board.get_ending_message());
                break;
            }
            case Board::Ending::TieBetweenBothPlayers: {
                const char* message1 = "Tie between both players!";
                draw_game_over_message(message1, game_layer->board.get_ending_message());
                break;
            }
            case Board::Ending::None:
                ASSERT(false, "Ending cannot be None");
        }

        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        const float window_width = ImGui::GetWindowSize().x;

        char time[32];
        gui_layer->timer.get_time_formatted(time);
        char time_label[64];
        sprintf(time_label, "Time: %s", time);

        const float text_width = ImGui::CalcTextSize(time_label).x;
        ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
        ImGui::Text("%s", time_label);

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            game_layer->board.phase = Board::Phase::None;
        }

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::EndPopup();
        HOVERING_GUI();
    }
    ImGui::PopFont();
}

void ImGuiLayer::draw_game_over_message(const char* message1, std::string_view message2) {
    ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();

    const float window_width = ImGui::GetWindowSize().x;
    float text_width;

    text_width = ImGui::CalcTextSize(message1).x;
    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    ImGui::Text("%s", message1); ImGui::SameLine();
    ImGui::Dummy(ImVec2(20.0f, 0.0f));

    text_width = ImGui::CalcTextSize(message2.data()).x;
    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    ImGui::Text("%s", message2.data()); ImGui::SameLine();
    ImGui::Dummy(ImVec2(20.0f, 0.0f));

    ImGui::Dummy(ImVec2(0.0f, 2.0f));
}

void ImGuiLayer::draw_about() {
    ImGui::PushFont(app->data.imgui_windows_font);
    ImGui::OpenPopup("About Nine Morris 3D");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("About Nine Morris 3D", nullptr, ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_AlwaysAutoResize)) {
        HOVERING_GUI();

        static bool deactivated = false;
        if (!deactivated) {
            game_layer->active = false;
            gui_layer->active = false;
            app->update_active_layers();

            deactivated = true;
        }

        ImGui::Text("A 3D implementation of the board game Nine Men's Morris");
        ImGui::Text("Version %u.%u.%u", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
        ImGui::Separator();
        ImGui::Text("All programming by:");
        ImGui::Text(u8"Simon Teodor Mărăcine - simonmara.dev@gmail.com");

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        const float window_width = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            show_about = false;
            deactivated = false;

            game_layer->active = true;
            gui_layer->active = true;
            app->update_active_layers();

            gui_layer->timer.reset_last_time(app->window->get_time());
        }

        ImGui::EndPopup();
    }
    ImGui::PopFont();
}

void ImGuiLayer::draw_could_not_load_game() {
    ImGui::PushFont(app->data.imgui_windows_font);
    ImGui::OpenPopup("Error Loading Game");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Error Loading Game", nullptr, ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_AlwaysAutoResize)) {
        HOVERING_GUI();

        static bool deactivated = false;
        if (!deactivated) {
            game_layer->active = false;
            gui_layer->active = false;
            app->update_active_layers();

            deactivated = true;
        }

        ImGui::Text("Could not load last game.");
        ImGui::Text("The save game file is either missing or is corrupted.");
        ImGui::Separator();
        ImGui::Text("%s", save_game_file_path.c_str());

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        const float window_width = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            show_could_not_load_game = false;
            deactivated = false;

            game_layer->active = true;
            gui_layer->active = true;
            app->update_active_layers();

            gui_layer->timer.reset_last_time(app->window->get_time());
        }

        ImGui::EndPopup();
    }
    ImGui::PopFont();
}

void ImGuiLayer::draw_no_last_game() {
    ImGui::PushFont(app->data.imgui_windows_font);
    ImGui::OpenPopup("No Last Game");

    ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("No Last Game", nullptr, ImGuiWindowFlags_NoResize
            | ImGuiWindowFlags_AlwaysAutoResize)) {
        HOVERING_GUI();

        static bool deactivated = false;
        if (!deactivated) {
            game_layer->active = false;
            gui_layer->active = false;
            app->update_active_layers();

            deactivated = true;
        }

        ImGui::Text("There is no last game saved.");

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        const float window_width = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            show_no_last_game = false;
            deactivated = false;

            game_layer->active = true;
            gui_layer->active = true;
            app->update_active_layers();

            gui_layer->timer.reset_last_time(app->window->get_time());
        }

        ImGui::EndPopup();
    }
    ImGui::PopFont();
}

#ifdef PLATFORM_GAME_DEBUG
void ImGuiLayer::draw_debug(float dt) {
    if (!show_about) {
        ImGui::Begin("Debug");
        ImGui::Text("FPS: %f", app->fps);
        ImGui::Text("Frame time (ms): %f", dt * 1000.0f);
        ImGui::Text("White pieces: %u", game_layer->board.white_pieces_count);
        ImGui::Text("Black pieces: %u", game_layer->board.black_pieces_count);
        ImGui::Text("Not placed white pieces: %u", game_layer->board.not_placed_white_pieces_count);
        ImGui::Text("Not placed black pieces: %u", game_layer->board.not_placed_black_pieces_count);
        ImGui::Text("White can jump: %s", game_layer->board.can_jump[0] ? "true" : "false");
        ImGui::Text("Black can jump: %s", game_layer->board.can_jump[1] ? "true" : "false");
        ImGui::Text("Phase: %d", static_cast<int>(game_layer->board.phase));
        ImGui::Text("Turn: %s", game_layer->board.turn == Board::Player::White ? "white" : "black");
        ImGui::Text("Should take piece: %s", game_layer->board.should_take_piece ? "true" : "false");
        ImGui::Text("Turns without mills: %u", game_layer->board.turns_without_mills);
        ImGui::Text("Undo history size: %lu", game_layer->board.undo_state_history->size());
        ImGui::Text("Redo history size: %lu", game_layer->board.redo_state_history->size());
        ImGui::Text("Hovered ID: %d", app->renderer->get_hovered_id());
        ImGui::Text("Hovered node: %p", game_layer->board.hovered_node);
        ImGui::Text("Hovered piece: %p", game_layer->board.hovered_piece);
        ImGui::Text("Selected piece: %p", game_layer->board.selected_piece);
        ImGui::Text("Next move: %s", game_layer->board.next_move ? "true" : "false");
        ImGui::Text("Game started: %s", game_layer->first_move ? "true" : "false");
        ImGui::End();

        ImGui::Begin("Light Settings");
        if (ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&app->renderer->light.position),
                -30.0f, 30.0f)) {
            app->renderer->light = app->renderer->light;
        }
        ImGui::SliderFloat3("Ambient color", reinterpret_cast<float*>(&app->renderer->light.ambient_color),
                0.0f, 1.0f);
        ImGui::SliderFloat3("Diffuse color", reinterpret_cast<float*>(&app->renderer->light.diffuse_color),
                0.0f, 1.0f);
        ImGui::SliderFloat3("Specular color", reinterpret_cast<float*>(&app->renderer->light.specular_color),
                0.0f, 1.0f);
        ImGui::End();

        // If you recompile shaders, uniforms that are set only once need to be reuploaded
        ImGui::Begin("Shaders");
        if (ImGui::Button("board_paint")) {
            app->data.board_paint_shader->recompile();
        }
        if (ImGui::Button("board")) {
            app->data.board_wood_shader->recompile();
        }
        if (ImGui::Button("node")) {
            app->data.node_shader->recompile();
        }
        if (ImGui::Button("origin")) {
            app->renderer->get_origin_shader()->recompile();
        }
        if (ImGui::Button("outline")) {
            app->renderer->get_outline_shader()->recompile();
        }
        if (ImGui::Button("piece")) {
            app->data.piece_shader->recompile();
        }
        if (ImGui::Button("quad2d")) {
            app->gui_renderer->get_quad2d_shader()->recompile();
        }
        if (ImGui::Button("quad3d")) {
            app->renderer->get_quad3d_shader()->recompile();
        }
        if (ImGui::Button("screen_quad")) {
            app->renderer->get_screen_quad_shader()->recompile();
        }
        if (ImGui::Button("shadow")) {
            app->renderer->get_shadow_shader()->recompile();
        }
        if (ImGui::Button("skybox")) {
            app->renderer->get_skybox_shader()->recompile();
        }
        if (ImGui::Button("text")) {
            app->gui_renderer->get_text_shader()->recompile();
        }
        ImGui::End();

        ImGui::Begin("Camera Debug");
        ImGui::Text("Position: %f, %f, %f", app->camera.get_position().x, app->camera.get_position().y,
                app->camera.get_position().z);
        ImGui::Text("Pitch: %f", app->camera.get_pitch());
        ImGui::Text("Yaw: %f", app->camera.get_yaw());
        ImGui::Text("Angle around point: %f", app->camera.get_angle_around_point());
        ImGui::Text("Distance to point: %f", app->camera.get_distance_to_point());
        ImGui::End();

        ImGui::Begin("Light Space Matrix");
        ImGui::SliderFloat("Left", &app->renderer->light_space.left, -10.0f, 10.0f);
        ImGui::SliderFloat("Right", &app->renderer->light_space.right, -10.0f, 10.0f);
        ImGui::SliderFloat("Bottom", &app->renderer->light_space.bottom, -10.0f, 10.0f);
        ImGui::SliderFloat("Top", &app->renderer->light_space.top, -10.0f, 10.0f);
        ImGui::SliderFloat("Near", &app->renderer->light_space.near, 0.1f, 2.0f);
        ImGui::SliderFloat("Far", &app->renderer->light_space.far, 2.0f, 50.0f);
        ImGui::SliderFloat("Light divisor", &app->renderer->light_space.light_divisor, 1.0f, 10.0f);
        ImGui::End();
    }
}
#endif

int ImGuiLayer::get_texture_quality_option(std::string_view option) {
    if (option == options::LOW) {
        return 0;
    } else if (option == options::NORMAL) {
        return 1;
    }

    ASSERT(false, "Invalid option");
    return -1;
}

int ImGuiLayer::get_skybox_option(std::string_view option) {
    if (option == options::FIELD) {
        return 0;
    } else if (option == options::AUTUMN) {
        return 1;
    }

    ASSERT(false, "Invalid option");
    return -1;
}
