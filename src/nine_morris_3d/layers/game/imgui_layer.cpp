#include <time.h>

#include <imgui.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_glfw.h>
#include <glm/gtc/matrix_transform.hpp>

#include "application/application.h"
#include "application/app.h"
#include "application/events.h"
#include "graphics/debug_opengl.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/framebuffer.h"
#include "other/logging.h"
#include "other/assets.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/save_load.h"
#include "nine_morris_3d/options.h"
#include "nine_morris_3d/board.h"

#define RESET_HOVERING_GUI() hovering_gui = false
#define HOVERING_GUI() hovering_gui = true

#define DEFAULT_BROWN ImVec4(0.6f, 0.35f, 0.12f, 1.0f)
#define DARK_BROWN ImVec4(0.4f, 0.25f, 0.10f, 1.0f)
#define LIGHT_BROWN ImVec4(0.68f, 0.42f, 0.12f, 1.0f)

void ImGuiLayer::on_attach() {
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigWindowsResizeFromEdges = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
#ifdef NDEBUG
    io.IniFilename = nullptr;
#endif

    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddText(u8"ă");

    ImVector<ImWchar> ranges;
    builder.BuildRanges(&ranges);

    io.FontDefault = io.Fonts->AddFontFromFileTTF(assets::path(assets::OPEN_SANS_FONT).c_str(),
            20.0f, nullptr, ranges.Data);
    info_font = io.Fonts->AddFontFromFileTTF(assets::path(assets::OPEN_SANS_FONT).c_str(), 16.0f);
    io.Fonts->Build();

    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_TitleBg] = DEFAULT_BROWN;
    colors[ImGuiCol_TitleBgActive] = DEFAULT_BROWN;
    colors[ImGuiCol_FrameBg] = DEFAULT_BROWN;
    colors[ImGuiCol_Button] = DARK_BROWN;
    colors[ImGuiCol_ButtonHovered] = DEFAULT_BROWN;
    colors[ImGuiCol_ButtonActive] = LIGHT_BROWN;
    colors[ImGuiCol_Header] = DARK_BROWN;
    colors[ImGuiCol_HeaderHovered] = DEFAULT_BROWN;

    ImGuiStyle& style = ImGui::GetStyle();
    style.FrameRounding = 8;
    style.WindowRounding = 8;
    style.ChildRounding = 8;
    style.PopupRounding = 8;

    ImGui_ImplOpenGL3_Init("#version 430 core");
    ImGui_ImplGlfw_InitForOpenGL(app->window->get_handle(), false);

    save_load::GameState state;
    try {
        save_load::load_game(state);
    } catch (const std::exception& e) {
        REL_ERROR("{}", e.what());
        REL_ERROR("Could not load game");
    }
    last_save_date = std::move(state.date);
}

void ImGuiLayer::on_detach() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    // These need to be resetted
    hovering_gui = false;
    can_undo = false;
    show_info = false;
    about_mode = false;
}

void ImGuiLayer::on_bind_layers() {
    game_layer = get_layer<GameLayer>(0, scene);
    gui_layer = get_layer<GuiLayer>(1, scene);
}

void ImGuiLayer::on_update(float dt) {
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(app->data.width, app->data.height);
    io.DeltaTime = dt;

    ImGui_ImplGlfw_NewFrame();
    ImGui_ImplOpenGL3_NewFrame();
    ImGui::NewFrame();

    RESET_HOVERING_GUI();

    if (!about_mode && ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game", nullptr, false)) {
                app->change_scene(0);

                DEB_INFO("Restarting game");
            }
            if (ImGui::MenuItem("Load Last", nullptr, false)) {
                scene->board.finalize_pieces_state();

                game_layer->load_game();
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", last_save_date.c_str());
            }
            if (ImGui::MenuItem("Save", nullptr, false)) {
                scene->board.finalize_pieces_state();

                save_load::GameState state;
                state.board = scene->board;
                state.camera = scene->camera;
                state.time = scene->timer.get_time_raw();

                const time_t current = time(nullptr);
                state.date = ctime(&current);

                save_load::save_game(state);

                last_save_date = state.date;
            }
            if (ImGui::MenuItem("Undo", nullptr, false, can_undo)) {
                scene->board.undo();

                if (scene->board.not_placed_pieces_count() == 18) {
                    can_undo = false;
                }
            }
            if (ImGui::MenuItem("Exit", nullptr, false)) {
                app->running = false;
            }

            ImGui::EndMenu();
            HOVERING_GUI();
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::MenuItem("VSync", nullptr, &app->options.vsync)) {
                if (app->options.vsync) {
                    app->window->set_vsync(app->options.vsync);

                    DEB_INFO("VSync enabled");
                } else {
                    app->window->set_vsync(app->options.vsync);

                    DEB_INFO("VSync disabled");
                }
            }
            if (ImGui::MenuItem("Save On Exit", nullptr, &app->options.save_on_exit)) {
                if (app->options.save_on_exit) {
                    DEB_INFO("The game will be saved on exit");
                } else {
                    DEB_INFO("The game will not be saved on exit");
                }
            }
            if (ImGui::BeginMenu("Anti-Aliasing", true)) {
                if (ImGui::RadioButton("No Anti-Aliasing", &app->options.samples, 1)) {
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
            }
            if (ImGui::BeginMenu("Texture Quality", true)) {
                static int quality = app->options.texture_quality == options::NORMAL ? 0 : 1;
                if (ImGui::RadioButton("Normal", &quality, 0)) {
                    game_layer->set_textures_quality(options::NORMAL);
                    app->options.texture_quality = options::NORMAL;

                    DEB_INFO("Textures set to {} quality", options::NORMAL);
                }
                if (ImGui::RadioButton("Low", &quality, 1)) {
                    game_layer->set_textures_quality(options::LOW);
                    app->options.texture_quality = options::LOW;

                    DEB_INFO("Textures set to {} quality", options::LOW);
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Skybox", true)) {
                static int skybox = app->options.skybox == options::FIELD ? 0 : 1;
                if (ImGui::RadioButton("Field", &skybox, 0)) {
                    game_layer->set_skybox(options::FIELD);
                    app->options.skybox = options::FIELD;

                    DEB_INFO("Skybox set to {}", options::FIELD);
                }
                if (ImGui::RadioButton("Autumn", &skybox, 1)) {
                    game_layer->set_skybox(options::AUTUMN);
                    app->options.skybox = options::AUTUMN;

                    DEB_INFO("Skybox set to {}", options::AUTUMN);
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Show Info", nullptr, &show_info)) {
                if (show_info) {
                    DEB_INFO("Show info");
                } else {
                    DEB_INFO("Hide info");
                }
            }
            if (ImGui::MenuItem("Custom Cursor", nullptr, &app->options.custom_cursor)) {
                if (app->options.custom_cursor) {
                    if (scene->board.should_take_piece) {
                        app->window->set_custom_cursor(CustomCursor::Cross);
                    } else {
                        app->window->set_custom_cursor(CustomCursor::Arrow);
                    }

                    DEB_INFO("Set custom cursor");
                } else {
                    app->window->set_custom_cursor(CustomCursor::None);

                    DEB_INFO("Set default cursor");
                }
            }

            ImGui::EndMenu();
            HOVERING_GUI();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About", nullptr, false)) {
                about_mode = true;
            }
            if (ImGui::MenuItem("Log Info", nullptr, false)) {
                logging::log_opengl_and_dependencies_info(logging::LogTarget::File);

                DEB_INFO("Logged OpenGL and dependencies info");
            }

            ImGui::EndMenu();
            HOVERING_GUI();
        }

        ImGui::EndMainMenuBar();
    }

    if (about_mode) {
        ImGui::OpenPopup("About Nine Morris 3D");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        if (ImGui::BeginPopupModal("About Nine Morris 3D", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            HOVERING_GUI();
            game_layer->active = false;
            gui_layer->active = false;

            float width;
            float height;
            float x_pos;
            float y_pos;

            if (static_cast<float>(app->get_width()) / static_cast<float>(app->get_height()) > 16.0f / 9.0f) {
                width = app->get_width();
                height = app->get_width() * (9.0f / 16.0f);
                x_pos = 0.0f;
                y_pos = (app->get_height() - height) / 2.0f;
            } else {
                height = app->get_height();
                width = app->get_height() * (16.0f / 9.0f);
                x_pos = (app->get_width() - width) / 2.0f;
                y_pos = 0.0f;
            }

            renderer::draw_quad_2d(glm::vec2(x_pos, y_pos), glm::vec2(width, height), app->storage->splash_screen_texture);

            ImGui::Text("A 3D implementation of the board game Nine Men's Morris");
            ImGui::Text("Version %d.%d.%d", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
            ImGui::Separator();
            ImGui::Text("All programming by:");
            ImGui::Text(u8"Simon Teodor Mărăcine - simonmara.dev@gmail.com");

            if (ImGui::Button("Ok", ImVec2(430, 0))) {
                ImGui::CloseCurrentPopup();
                about_mode = false;

                game_layer->active = true;
                gui_layer->active = true;
            }

            ImGui::EndPopup();
        }
    }

    if (scene->board.not_placed_pieces_count() < 18) {
        can_undo = true;
    }

    if (scene->board.phase == Board::Phase::GameOver) {
        ImGui::OpenPopup("Game Over");

        ImVec2 center = ImGui::GetMainViewport()->GetCenter();
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
        ImGuiStyle& style = ImGui::GetStyle();
        style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

        if (ImGui::BeginPopupModal("Game Over", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
            switch (scene->board.ending) {
                case Board::Ending::WinnerWhite: {
                    const char* message = "White player wins!";
                    const float window_width = ImGui::GetWindowSize().x;
                    const float text_width = ImGui::CalcTextSize(message).x;
                    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
                    ImGui::Text("%s", message);
                    break;
                }
                case Board::Ending::WinnerBlack: {
                    const char* message = "Black player wins!";
                    const float window_width = ImGui::GetWindowSize().x;
                    const float text_width = ImGui::CalcTextSize(message).x;
                    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
                    ImGui::Text("%s", message);
                    break;
                }
                case Board::Ending::TieBetweenBothPlayers: {
                    const char* message = "Tie between both players!";
                    const float window_width = ImGui::GetWindowSize().x;
                    const float text_width = ImGui::CalcTextSize(message).x;
                    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
                    ImGui::Text("%s", message);
                    break;
                }
                case Board::Ending::None:
                    assert(false);
            }

            if (ImGui::Button("Ok", ImVec2(200, 0))) {
                ImGui::CloseCurrentPopup();
                scene->board.phase = Board::Phase::None;
            }

            ImGui::EndPopup();
            HOVERING_GUI();
        }
    }

    if (show_info && !about_mode) {
        ImGui::PushFont(info_font);
        ImGui::Begin("Info");
        ImGui::Text("FPS: %f", app->fps);
        ImGui::Text("OpenGL: %s", debug_opengl::get_opengl_version());
        ImGui::Text("GLSL: %s", debug_opengl::get_glsl_version());
        ImGui::Text("Vendor: %s", debug_opengl::get_vendor());
        ImGui::Text("Renderer: %s", debug_opengl::get_renderer());
        ImGui::End();
        ImGui::PopFont();
        HOVERING_GUI();
    }

#ifndef NDEBUG
    if (!about_mode) {
        ImGui::Begin("Debug");
        ImGui::Text("FPS: %f", app->fps);
        ImGui::Text("Frame time (ms): %f", dt * 1000.0f);
        ImGui::Text("White pieces: %u", scene->board.white_pieces_count);
        ImGui::Text("Black pieces: %u", scene->board.black_pieces_count);
        ImGui::Text("Not placed white pieces: %u", scene->board.not_placed_white_pieces_count);
        ImGui::Text("Not placed black pieces: %u", scene->board.not_placed_black_pieces_count);
        ImGui::Text("White can jump: %s", scene->board.can_jump[0] ? "true" : "false");
        ImGui::Text("Black can jump: %s", scene->board.can_jump[1] ? "true" : "false");
        ImGui::Text("Phase: %d", static_cast<int>(scene->board.phase));
        ImGui::Text("Turn: %s", scene->board.turn == Board::Player::White ? "white" : "black");
        ImGui::Text("Should take piece: %s", scene->board.should_take_piece ? "true" : "false");
        ImGui::Text("Turns without mills: %u", scene->board.turns_without_mills);
        ImGui::Text("History size: %lu", scene->board.state_history->size());
        ImGui::Text("Hovered ID: %d", scene->hovered_id);
        ImGui::Text("Hovered node: %p", scene->board.hovered_node);
        ImGui::Text("Hovered piece: %p", scene->board.hovered_piece);
        ImGui::Text("Selected piece: %p", scene->board.selected_piece);
        ImGui::Text("Next move: %s", scene->board.next_move ? "true" : "false");
        ImGui::End();

        ImGui::Begin("Debug Settings");
        if (ImGui::SliderFloat3("Light position", reinterpret_cast<float*>(&scene->light.position), -30.0f, 30.0f)) {
            game_layer->setup_light();
        }
        if (ImGui::SliderFloat3("Light ambient color", reinterpret_cast<float*>(&scene->light.ambient_color), 0.0f, 1.0f)) {
            game_layer->setup_light();
        }
        if (ImGui::SliderFloat3("Light diffuse color", reinterpret_cast<float*>(&scene->light.diffuse_color), 0.0f, 1.0f)) {
            game_layer->setup_light();
        }
        if (ImGui::SliderFloat3("Light specular color", reinterpret_cast<float*>(&scene->light.specular_color), 0.0f, 1.0f)) {
            game_layer->setup_light();
        }
        ImGui::End();
    }
#endif

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void ImGuiLayer::on_draw() {

}

void ImGuiLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher (event);

    dispatcher.dispatch<MouseScrolledEvent>(MouseScrolled, BIND(ImGuiLayer::on_mouse_scrolled));
    dispatcher.dispatch<MouseMovedEvent>(MouseMoved, BIND(ImGuiLayer::on_mouse_moved));
    dispatcher.dispatch<MouseButtonPressedEvent>(MouseButtonPressed, BIND(ImGuiLayer::on_mouse_button_pressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(MouseButtonReleased, BIND(ImGuiLayer::on_mouse_button_released));
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(ImGuiLayer::on_window_resized));
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
    io.MouseDown[event.button] = true;

    return hovering_gui;
}

bool ImGuiLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    ImGuiIO& io = ImGui::GetIO();
    io.MouseDown[event.button] = false;

    return hovering_gui;
}

bool ImGuiLayer::on_window_resized(events::WindowResizedEvent& event) {
    scene->camera.update_projection(static_cast<float>(event.width), static_cast<float>(event.height));

    return false;
}
