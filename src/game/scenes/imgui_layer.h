#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/save_load.h"
#include "game/game_options.h"
#include "game/constants.h"
#include "imgui_style/colors.h"
#include "other/data.h"

#define RESET_HOVERING_GUI() hovering_gui = false
#define HOVERING_GUI() hovering_gui = true

static int get_skybox_option(std::string_view option) {
    if (option == game_options::FIELD) {
        return 0;
    } else if (option == game_options::AUTUMN) {
        return 1;
    }

    ASSERT(false, "Invalid option");
    return -1;
}

template<typename SceneType>
struct ImGuiLayer {
    ImGuiLayer() = default;
    ImGuiLayer(Application* app, SceneType* scene);
    ~ImGuiLayer() = default;

    void update();
    void reset();
    void initialize();

    void draw_menu_bar();
    void draw_game_over();
    void draw_game_over_message(std::string_view message1, std::string_view message2);
    void draw_about();
    void draw_could_not_load_game();
    void draw_no_last_game();

#ifdef PLATFORM_GAME_DEBUG
    void draw_debug();
#endif

    Application* app = nullptr;
    SceneType* scene = nullptr;

    bool hovering_gui = false;
    bool can_undo = false;
    bool can_redo = false;

    bool show_info = false;
    bool show_about = false;
    bool show_could_not_load_game = false;
    bool show_no_last_game = false;

    std::string last_save_game_date = save_load::NO_LAST_GAME;

    std::string info_file_path;
    std::string save_game_file_path;

    ImGuiWindowFlags window_flags = 0;
};

template<typename SceneType>
ImGuiLayer<SceneType>::ImGuiLayer(Application* app, SceneType* scene)
    : app(app), scene(scene) {
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigWindowsResizeFromEdges = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
#ifdef PLATFORM_GAME_RELEASE
    io.IniFilename = nullptr;
#endif
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;

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
    colors[ImGuiCol_Tab] = DARK_BROWN;
    colors[ImGuiCol_TabHovered] = LIGHT_BROWN;
    colors[ImGuiCol_TabActive] = DEFAULT_BROWN;

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.FrameRounding = 8;
    style.WindowRounding = 8;
    style.ChildRounding = 8;
    style.PopupRounding = 8;
    style.GrabRounding = 8;
}

template<typename SceneType>
void ImGuiLayer<SceneType>::update() {
    save_load::SavedGame saved_game;
    try {
        save_load::load_game_from_file(saved_game);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARN("{}", e.what());
        save_load::handle_save_file_not_open_error(app->data().application_name);
        REL_WARN("Could not load game");
    } catch (const save_load::SaveFileError& e) {
        REL_WARN("{}", e.what());  // TODO maybe delete file
        REL_WARN("Could not load game");
    }
    last_save_game_date = std::move(saved_game.date);
    DEB_INFO("Checked last saved game");

    try {
        info_file_path = paths::path_for_logs(app->data().info_file_name);
        save_game_file_path = paths::path_for_saved_data(save_load::SAVE_GAME_FILE);
    } catch (const user_data::UserNameError& e) {
        REL_ERROR("{}", e.what());

        info_file_path = app->data().info_file_name;
        save_game_file_path = save_load::SAVE_GAME_FILE;
    }
}

template<typename SceneType>
void ImGuiLayer<SceneType>::reset() {
    hovering_gui = false;
    can_undo = false;
    can_redo = false;

    show_info = false;
    show_about = false;
    show_could_not_load_game = false;
    show_no_last_game = false;
}

template<typename SceneType>
void ImGuiLayer<SceneType>::draw_menu_bar() {
    auto& data = app->user_data<Data>();

    RESET_HOVERING_GUI();

    if (!show_about && ImGui::BeginMainMenuBar()) {
        const bool can_change = scene->game.state == GameState::HumanThinkingMove;

        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game", nullptr, false, can_change)) {
                app->change_scene("game");

                DEB_INFO("Restarting game");
            }
            if (ImGui::MenuItem("Load Last Game", nullptr, false, can_change)) {
                // scene->board.finalize_pieces_state();  // FIXME

                // game_layer->load_game();  // FIXME
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", last_save_game_date.c_str());
            }
            if (ImGui::MenuItem("Save Game", nullptr, false)) {
                // scene->board.finalize_pieces_state();  // FIXME

                save_load::SavedGame saved_game;
                saved_game.board = scene->board;
                saved_game.camera = scene->camera;
                saved_game.time = scene->timer.get_time_raw();

                time_t current;
                time(&current);
                saved_game.date = ctime(&current);

                saved_game.undo_redo_state = scene->undo_redo_state;

                saved_game.white_player = scene->game.white_player;
                saved_game.black_player = scene->game.black_player;

                try {
                    save_load::save_game_to_file(saved_game);
                } catch (const save_load::SaveFileNotOpenError& e) {
                    REL_ERROR("{}", e.what());
                    save_load::handle_save_file_not_open_error(app->data().application_name);
                    REL_ERROR("Could not save game");
                } catch (const save_load::SaveFileError& e) {
                    REL_ERROR("{}", e.what());
                    REL_ERROR("Could not save game");
                }

                last_save_game_date = std::move(saved_game.date);
            }
            if (ImGui::BeginMenu("Players", can_change)) {
                if (ImGui::BeginMenu("White")) {
                    static int option = 0;
                    if (ImGui::RadioButton("Human", &option, 0)) {
                        scene->game.white_player = GamePlayer::Human;
                        scene->game.reset_player(GamePlayer::Human);

                        DEB_DEBUG("Set white player to human");
                    }
                    if (ImGui::RadioButton("Computer", &option, 1)) {
                        scene->game.white_player = GamePlayer::Computer;
                        scene->game.reset_player(GamePlayer::Computer);

                        DEB_DEBUG("Set white player to computer");
                    }

                    ImGui::EndMenu();
                    HOVERING_GUI();
                }
                if (ImGui::BeginMenu("Black")) {
                    static int option = 1;
                    if (ImGui::RadioButton("Human", &option, 0)) {
                        scene->game.black_player = GamePlayer::Human;
                        scene->game.reset_player(GamePlayer::Human);

                        DEB_DEBUG("Set black player to human");
                    }
                    if (ImGui::RadioButton("Computer", &option, 1)) {
                        scene->game.black_player = GamePlayer::Computer;
                        scene->game.reset_player(GamePlayer::Computer);

                        DEB_DEBUG("Set black player to computer");
                    }

                    ImGui::EndMenu();
                    HOVERING_GUI();
                }

                ImGui::EndMenu();
                HOVERING_GUI();
            }
            if (ImGui::MenuItem("Undo", nullptr, false, can_undo && can_change)) {
                const bool undid_game_over = false;  // FIXME game_layer->undo();

                if (scene->undo_redo_state.undo.empty()) {
                    can_undo = false;
                }

                if (undid_game_over) {
                    scene->timer.start(app->window->get_time());
                }
            }
            if (ImGui::MenuItem("Redo", nullptr, false, can_redo && can_change)) {
                const bool redid_game_over = false;  // FIXME game_layer->redo();

                if (scene->undo_redo_state.redo.empty()) {
                    can_redo = false;
                }

                if (redid_game_over) {
                    scene->timer.stop();
                    scene->board.phase = BoardPhase::GameOver;
                }
            }
            if (ImGui::MenuItem("Exit To Launcher", nullptr, false)) {
                app->running = false;
                app->exit_code = 1;
            }
            if (ImGui::MenuItem("Exit", nullptr, false)) {
                app->running = false;
            }

            ImGui::EndMenu();
            HOVERING_GUI();
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::BeginMenu("Graphics")) {
                if (ImGui::MenuItem("VSync", nullptr, &data.options.vsync)) {
                    if (data.options.vsync) {
                        app->window->set_vsync(data.options.vsync);

                        DEB_INFO("VSync enabled");
                    } else {
                        app->window->set_vsync(data.options.vsync);

                        DEB_INFO("VSync disabled");
                    }
                }
                if (ImGui::MenuItem("Custom Cursor", nullptr, &data.options.custom_cursor)) {
                    if (data.options.custom_cursor) {
                        if (scene->board.must_take_piece) {
                            app->window->set_cursor(data.cross_cursor);
                        } else {
                            app->window->set_cursor(data.arrow_cursor);
                        }

                        DEB_INFO("Set custom cursor");
                    } else {
                        app->window->set_cursor(0);

                        DEB_INFO("Set default cursor");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI();
            }
            if (ImGui::MenuItem("Save On Exit", nullptr, &data.options.save_on_exit)) {
                if (data.options.save_on_exit) {
                    DEB_INFO("The game will be saved on exit");
                } else {
                    DEB_INFO("The game will not be saved on exit");
                }
            }
            if (ImGui::BeginMenu("Skybox")) {
                static int dummy = get_skybox_option(data.options.skybox);

                if (ImGui::RadioButton("Field", &dummy, 0)) {
                    // game_layer->set_skybox(options::FIELD);  // FIXME

                    DEB_INFO("Skybox set to {}", game_options::FIELD);
                }
                if (ImGui::RadioButton("Autumn", &dummy, 1)) {
                    // game_layer->set_skybox(options::AUTUMN);  // FIXME

                    DEB_INFO("Skybox set to {}", game_options::AUTUMN);
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
                if (ImGui::SliderFloat("##", &data.options.sensitivity, 0.5f, 2.0f, "%.01f", ImGuiSliderFlags_Logarithmic)) {
                    scene->camera.sensitivity = data.options.sensitivity;

                    DEB_INFO("Changed camera sensitivity to {}", scene->camera.sensitivity);
                }
                ImGui::PopItemWidth();

                ImGui::EndMenu();
                HOVERING_GUI();
            }
            if (ImGui::BeginMenu("User Interface")) {
                if (ImGui::MenuItem("Hide Timer", nullptr, &data.options.hide_timer)) {
                    if (data.options.hide_timer) {
                        app->gui_renderer->remove_widget(scene->timer_text);

                        DEB_INFO("Hide timer");
                    } else {
                        app->gui_renderer->add_widget(scene->timer_text);

                        DEB_INFO("Show timer");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI();
            }
            static bool labeled_board = data.options.labeled_board;

            if (ImGui::MenuItem("Labeled Board", nullptr, &labeled_board)) {
                if (labeled_board) {
                    // game_layer->set_labeled_board_texture(labeled_board);  // FIXME

                    DEB_INFO("Labeled board");
                } else {
                    // game_layer->set_labeled_board_texture(labeled_board);  // FIXME

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
                logging::log_opengl_and_dependencies_info(logging::LogTarget::File, app->data().info_file_name);

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
}

template<typename SceneType>
void ImGuiLayer<SceneType>::draw_game_over() {
    ImGui::PushFont(app->user_data<Data>().imgui_windows_font);
    ImGui::OpenPopup("Game Over");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Game Over", nullptr, window_flags)) {
        ImGui::Dummy(ImVec2(0.0f, 4.0f));

        switch (scene->board.ending.type) {
            case BoardEnding::WinnerWhite: {
                const char* message1 = "White player wins!";
                draw_game_over_message(message1, scene->board.ending.message);
                break;
            }
            case BoardEnding::WinnerBlack: {
                const char* message1 = "Black player wins!";
                draw_game_over_message(message1, scene->board.ending.message);
                break;
            }
            case BoardEnding::TieBetweenBothPlayers: {
                const char* message1 = "Tie between both players!";
                draw_game_over_message(message1, scene->board.ending.message);
                break;
            }
            case BoardEnding::None:
                ASSERT(false, "Ending cannot be None");
        }

        ImGui::Separator();
        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        const float window_width = ImGui::GetWindowSize().x;

        char time[32];
        scene->timer.get_time_formatted(time);
        char time_label[64];
        sprintf(time_label, "Time: %s", time);

        const float text_width = ImGui::CalcTextSize(time_label).x;
        ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
        ImGui::Text("%s", time_label);

        ImGui::Dummy(ImVec2(0.0f, 8.0f));

        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            scene->board.phase = BoardPhase::None;
        }

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::EndPopup();
        HOVERING_GUI();
    }
    ImGui::PopFont();
}

template<typename SceneType>
void ImGuiLayer<SceneType>::draw_game_over_message(std::string_view message1, std::string_view message2) {
    ImGui::Dummy(ImVec2(20.0f, 0.0f)); ImGui::SameLine();

    const float window_width = ImGui::GetWindowSize().x;
    float text_width;

    text_width = ImGui::CalcTextSize(message1.data()).x;
    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    ImGui::Text("%s", message1.data()); ImGui::SameLine();
    ImGui::Dummy(ImVec2(20.0f, 0.0f));

    text_width = ImGui::CalcTextSize(message2.data()).x;
    ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
    ImGui::Text("%s", message2.data()); ImGui::SameLine();
    ImGui::Dummy(ImVec2(20.0f, 0.0f));

    ImGui::Dummy(ImVec2(0.0f, 2.0f));
}

template<typename SceneType>
void ImGuiLayer<SceneType>::draw_about() {
    ImGui::PushFont(app->user_data<Data>().imgui_windows_font);
    ImGui::OpenPopup("About Nine Morris 3D");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("About Nine Morris 3D", nullptr, window_flags)) {
        HOVERING_GUI();

        // static bool deactivated = false;
        // if (!deactivated) {
        //     game_layer->active = false;
        //     gui_layer->active = false;
        //     app->update_active_layers();

        //     deactivated = true;
        // }

        ImGui::Text("A 3D implementation of the board game Nine Men's Morris");
        ImGui::Text("Version %u.%u.%u", app->data().version_major, app->data().version_minor, app->data().version_patch);
        ImGui::Separator();
        ImGui::Text("All programming by:");
        ImGui::Text(u8"Simon Teodor Mărăcine - simonmara.dev@gmail.com");

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        const float window_width = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            show_about = false;
            // deactivated = false;

            // game_layer->active = true;
            // gui_layer->active = true;
            // app->update_active_layers();

            scene->timer.reset_last_time(app->window->get_time());
        }

        ImGui::EndPopup();
    }
    ImGui::PopFont();
}

template<typename SceneType>
void ImGuiLayer<SceneType>::draw_could_not_load_game() {
    ImGui::PushFont(app->user_data<Data>().imgui_windows_font);
    ImGui::OpenPopup("Error Loading Game");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Error Loading Game", nullptr, window_flags)) {
        HOVERING_GUI();

        // static bool deactivated = false;
        // if (!deactivated) {
        //     game_layer->active = false;
        //     gui_layer->active = false;
        //     app->update_active_layers();

        //     deactivated = true;
        // }

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
            // deactivated = false;

            // game_layer->active = true;
            // gui_layer->active = true;
            // app->update_active_layers();

            scene->timer.reset_last_time(app->window->get_time());
        }

        ImGui::EndPopup();
    }
    ImGui::PopFont();
}

template<typename SceneType>
void ImGuiLayer<SceneType>::draw_no_last_game() {
    ImGui::PushFont(app->user_data<Data>().imgui_windows_font);
    ImGui::OpenPopup("No Last Game");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("No Last Game", nullptr, window_flags)) {
        HOVERING_GUI();

        // static bool deactivated = false;
        // if (!deactivated) {
        //     game_layer->active = false;
        //     gui_layer->active = false;
        //     app->update_active_layers();

        //     deactivated = true;
        // }

        ImGui::Text("There is no last game saved.");

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        const float window_width = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            show_no_last_game = false;
            // deactivated = false;

            // game_layer->active = true;
            // gui_layer->active = true;
            // app->update_active_layers();

            scene->timer.reset_last_time(app->window->get_time());
        }

        ImGui::EndPopup();
    }
    ImGui::PopFont();
}

#ifdef PLATFORM_GAME_DEBUG
template<typename SceneType>
void ImGuiLayer<SceneType>::draw_debug() {
    if (!show_about) {
        ImGui::Begin("Debug");
        ImGui::Text("FPS: %f", app->get_fps());
        ImGui::Text("Frame time (ms): %f", app->get_delta() * 1000.0f);
        ImGui::Text("White pieces: %u", scene->board.white_pieces_count);
        ImGui::Text("Black pieces: %u", scene->board.black_pieces_count);
        ImGui::Text("Not placed white pieces: %u", scene->board.not_placed_pieces_count);
        ImGui::Text("White can jump: %s", scene->board.can_jump[0] ? "true" : "false");
        ImGui::Text("Black can jump: %s", scene->board.can_jump[1] ? "true" : "false");
        ImGui::Text("Phase: %d", static_cast<int>(scene->board.phase));
        ImGui::Text("Turn: %s", scene->board.turn == BoardPlayer::White ? "white" : "black");
        ImGui::Text("Should take piece: %s", scene->board.must_take_piece ? "true" : "false");
        ImGui::Text("Turns without mills: %u", scene->board.turns_without_mills);
        ImGui::Text("Undo history size: %lu", scene->undo_redo_state.undo.size());
        ImGui::Text("Redo history size: %lu", scene->undo_redo_state.redo.size());
        ImGui::Text("Hovered ID: %d", app->renderer->get_hovered_id());
        ImGui::Text("Hovered node: %zu", scene->board.hovered_node);
        ImGui::Text("Hovered piece: %zu", scene->board.hovered_piece);
        ImGui::Text("Selected piece: %zu", scene->board.selected_piece);
        ImGui::Text("Next move: %s", scene->board.next_move ? "true" : "false");
        ImGui::Text("Game started: %s", scene->made_first_move ? "true" : "false");
        ImGui::End();

        ImGui::Begin("Game");
        std::string state;
        switch (scene->game.state) {
            case GameState::MaybeNextPlayer:
                state = "MaybeNextPlayer";
                break;
            case GameState::HumanBeginMove:
                state = "HumanBeginMove";
                break;
            case GameState::HumanThinkingMove:
                state = "HumanThinkingMove";
                break;
            case GameState::HumanDoingMove:
                state = "HumanDoingMove";
                break;
            case GameState::HumanEndMove:
                state = "HumanEndMove";
                break;
            case GameState::ComputerBeginMove:
                state = "ComputerBeginMove";
                break;
            case GameState::ComputerThinkingMove:
                state = "ComputerThinkingMove";
                break;
            case GameState::ComputerDoingMove:
                state = "ComputerDoingMove";
                break;
            case GameState::ComputerEndMove:
                state = "ComputerEndMove";
                break;
        }
        ImGui::Text("State: %s", state.c_str());
        ImGui::Text("White player: %s", scene->game.white_player == GamePlayer::Human ? "Human" : "Computer");
        ImGui::Text("Black player: %s", scene->game.black_player == GamePlayer::Human ? "Human" : "Computer");
        ImGui::End();

        ImGui::Begin("Light Settings");
        ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&app->renderer->light.position), -30.0f, 30.0f);
        ImGui::SliderFloat3("Ambient color", reinterpret_cast<float*>(&app->renderer->light.ambient_color), 0.0f, 1.0f);
        ImGui::SliderFloat3("Diffuse color", reinterpret_cast<float*>(&app->renderer->light.diffuse_color), 0.0f, 1.0f);
        ImGui::SliderFloat3("Specular color", reinterpret_cast<float*>(&app->renderer->light.specular_color), 0.0f, 1.0f);
        ImGui::End();

        // If you recompile shaders, uniforms that are set only once need to be reuploaded
        ImGui::Begin("Shaders");
        if (ImGui::Button("board_paint")) {
            // app->data.board_paint_shader->recompile();  // FIXME this
        }
        if (ImGui::Button("board")) {
            // app->data.board_wood_shader->recompile();
        }
        if (ImGui::Button("node")) {
            // app->data.node_shader->recompile();
        }
        if (ImGui::Button("origin")) {
            // app->renderer->get_origin_shader()->recompile();
        }
        if (ImGui::Button("outline")) {
            // app->renderer->get_outline_shader()->recompile();
        }
        if (ImGui::Button("piece")) {
            // app->data.piece_shader->recompile();
        }
        if (ImGui::Button("quad2d")) {
            // app->gui_renderer->get_quad2d_shader()->recompile();
        }
        if (ImGui::Button("quad3d")) {
            // app->renderer->get_quad3d_shader()->recompile();
        }
        if (ImGui::Button("screen_quad")) {
            // app->renderer->get_screen_quad_shader()->recompile();
        }
        if (ImGui::Button("shadow")) {
            // app->renderer->get_shadow_shader()->recompile();
        }
        if (ImGui::Button("skybox")) {
            // app->renderer->get_skybox_shader()->recompile();
        }
        if (ImGui::Button("text")) {
            // app->gui_renderer->get_text_shader()->recompile();
        }
        ImGui::End();

        const glm::vec3& position = scene->camera.get_position();
        ImGui::Begin("Camera Debug");
        ImGui::Text("Position: %f, %f, %f", position.x, position.y, position.z);
        ImGui::Text("Pitch: %f", scene->camera.get_pitch());
        ImGui::Text("Yaw: %f", scene->camera.get_yaw());
        ImGui::Text("Angle around point: %f", scene->camera.get_angle_around_point());
        ImGui::Text("Distance to point: %f", scene->camera.get_distance_to_point());
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
