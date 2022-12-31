#pragma once

#include <engine/engine_application.h>
#include <engine/engine_audio.h>
#include <engine/engine_other.h>

#include "game/scenes/common.h"
#include "game/save_load.h"
#include "game/game_options.h"
#include "other/constants.h"
#include "other/data.h"

#define RESET_HOVERING_GUI() hovering_gui = false;
#define HOVERING_GUI() hovering_gui = true;

template<typename S, typename B>
class ImGuiLayer {
public:
    ImGuiLayer() = default;
    ImGuiLayer(Application* app, S* scene);
    ~ImGuiLayer() = default;

    void update();
    void reset();

    void draw_menu_bar();
    void draw_info();
    void draw_game_over();
    void draw_about();
    void draw_could_not_load_game();
    void draw_no_last_game();

#ifdef NM3D_PLATFORM_DEBUG
    void draw_debug();
#endif

    bool hovering_gui = false;
    bool can_undo = false;
    bool can_redo = false;

    bool show_info = false;
    bool show_about = false;
    bool show_could_not_load_game = false;
    bool show_no_last_game = false;
private:
    void draw_game_over_message(std::string_view message1, std::string_view message2);

    Application* app = nullptr;
    S* scene = nullptr;

    std::string last_save_game_date = save_load::NO_LAST_GAME;

    std::string info_file_path;
    std::string save_game_file_path;

    ImGuiWindowFlags window_flags = 0;

#ifdef NM3D_PLATFORM_DEBUG
    static constexpr size_t FRAMES_SIZE = 100;
    std::vector<float> frames = std::vector<float> {FRAMES_SIZE};
    size_t index = 0;
#endif
};

template<typename S, typename B>
ImGuiLayer<S, B>::ImGuiLayer(Application* app, S* scene)
    : app(app), scene(scene) {
    ImGuiIO& io = ImGui::GetIO();
    io.BackendFlags |= ImGuiBackendFlags_HasMouseCursors;
    io.BackendFlags |= ImGuiBackendFlags_HasSetMousePos;
    io.ConfigWindowsMoveFromTitleBarOnly = true;
    io.ConfigWindowsResizeFromEdges = false;
    io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
#ifdef NM3D_PLATFORM_RELEASE
    io.IniFilename = nullptr;
#endif
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_AlwaysAutoResize;
    window_flags |= ImGuiWindowFlags_NoCollapse;

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
    style.FrameRounding = 4;
    style.WindowRounding = 6;
    style.ChildRounding = 6;
    style.PopupRounding = 6;
    style.GrabRounding = 6;
    style.GrabMinSize = 12;
    style.FramePadding = ImVec2(5.0f, 4.0f);
}

template<typename S, typename B>
void ImGuiLayer<S, B>::update() {
    save_load::SavedGame<B> saved_game;
    try {
        save_load::load_game_from_file(saved_game);
    } catch (const save_load::SaveFileNotOpenError& e) {
        REL_WARNING("Could not load game: {}", e.what());
    } catch (const save_load::SaveFileError& e) {
        REL_WARNING("Could not load game: {}", e.what());  // TODO maybe delete file
    }
    last_save_game_date = std::move(saved_game.date);
    DEB_INFO("Checked last saved game");

    info_file_path = file_system::path_for_logs(logging::get_info_file());
    save_game_file_path = file_system::path_for_saved_data(save_load::SAVE_GAME_FILE);
}

template<typename S, typename B>
void ImGuiLayer<S, B>::reset() {
    hovering_gui = false;
    can_undo = false;
    can_redo = false;

    show_info = false;
    show_about = false;
    show_could_not_load_game = false;
    show_no_last_game = false;
}

template<typename S, typename B>
void ImGuiLayer<S, B>::draw_menu_bar() {
    auto& data = app->user_data<Data>();

    RESET_HOVERING_GUI();

    if (!show_about && ImGui::BeginMainMenuBar()) {
        const bool can_change = scene->game.state == GameState::HumanThinkingMove;
        const bool can_undo_redo = (
            scene->game.state == GameState::HumanThinkingMove
            || scene->game.state == GameState::ComputerThinkingMove
        );

        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game", nullptr, false, can_change)) {
                app->change_scene("standard_game");  // FIXME this

                DEB_INFO("Restarting game");
            }
            if (ImGui::MenuItem("Load Last Game", nullptr, false, can_change)) {
                if (last_save_game_date == save_load::NO_LAST_GAME) {
                    show_no_last_game = true;
                } else {
                    scene->load_game();
                }
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", last_save_game_date.c_str());
            }
            if (ImGui::MenuItem("Save Game", nullptr, false)) {
                scene->save_game();

                time_t current;  // TODO this time might be quite later
                time(&current);
                last_save_game_date = ctime(&current);
            }
            if (ImGui::BeginMenu("Game", can_change)) {
                static int option = 0;

                if (ImGui::RadioButton("Standard Game", &option, 0)) {
                    app->change_scene("standard_game");
                }
                if (ImGui::RadioButton("Jump Variant", &option, 1)) {
                    app->change_scene("jump_variant");
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::BeginMenu("Players", can_change)) {
                if (ImGui::BeginMenu("White")) {
                    static int option = data.options.white_player;

                    if (ImGui::RadioButton("Human", &option, 1)) {
                        data.options.white_player = game_options::HUMAN;
                        scene->game.white_player = GamePlayer::Human;
                        scene->game.reset_player(GamePlayer::Human);

                        DEB_DEBUG("Set white player to human");
                    }
                    if (ImGui::RadioButton("Computer", &option, 2)) {
                        data.options.white_player = game_options::COMPUTER;
                        scene->game.white_player = GamePlayer::Computer;
                        scene->game.reset_player(GamePlayer::Computer);

                        DEB_DEBUG("Set white player to computer");
                    }

                    ImGui::EndMenu();
                    HOVERING_GUI()
                }
                if (ImGui::BeginMenu("Black")) {
                    static int option = data.options.black_player;

                    if (ImGui::RadioButton("Human", &option, 1)) {
                        data.options.black_player = game_options::HUMAN;
                        scene->game.black_player = GamePlayer::Human;
                        scene->game.reset_player(GamePlayer::Human);

                        DEB_DEBUG("Set black player to human");
                    }
                    if (ImGui::RadioButton("Computer", &option, 2)) {
                        data.options.black_player = game_options::COMPUTER;
                        scene->game.black_player = GamePlayer::Computer;
                        scene->game.reset_player(GamePlayer::Computer);

                        DEB_DEBUG("Set black player to computer");
                    }

                    ImGui::EndMenu();
                    HOVERING_GUI()
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::MenuItem("Undo", nullptr, false, can_undo && can_undo_redo)) {
                scene->undo();
                can_undo = scene->undo_redo_state.undo.size() > 0;
                can_redo = scene->undo_redo_state.redo.size() > 0;
            }
            if (ImGui::MenuItem("Redo", nullptr, false, can_redo && can_undo_redo)) {
                scene->redo();
                can_undo = scene->undo_redo_state.undo.size() > 0;
                can_redo = scene->undo_redo_state.redo.size() > 0;
            }
            if (ImGui::MenuItem("Exit To Launcher", nullptr, false)) {
                app->running = false;
                app->exit_code = 1;
            }
            if (ImGui::MenuItem("Exit", nullptr, false)) {
                app->running = false;
            }

            ImGui::EndMenu();
            HOVERING_GUI()
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
                HOVERING_GUI()
            }
            if (ImGui::BeginMenu("Audio")) {
                if (ImGui::BeginMenu("Master Volume")) {
                    ImGui::PushItemWidth(100.0f);
                    if (ImGui::SliderFloat("##", &data.options.master_volume, 0.0f, 1.0f, "%.01f")) {
                        app->openal->get_listener().set_gain(data.options.master_volume);

                        DEB_INFO("Changed master volume to {}", data.options.master_volume);
                    }
                    ImGui::PopItemWidth();

                    ImGui::EndMenu();
                    HOVERING_GUI()
                }
                if (ImGui::BeginMenu("Music Volume")) {
                    ImGui::PushItemWidth(100.0f);
                    if (ImGui::SliderFloat("##", &data.options.music_volume, 0.0f, 1.0f, "%.01f")) {
                        music::set_music_gain(data.options.music_volume);

                        DEB_INFO("Changed music volume to {}", data.options.music_volume);
                    }
                    ImGui::PopItemWidth();

                    ImGui::EndMenu();
                    HOVERING_GUI()
                }
                if (ImGui::MenuItem("Enable Music", nullptr, &data.options.enable_music)) {
                    if (data.options.enable_music) {
                        music::play_music_track(scene->current_music_track);

                        DEB_INFO("Enabled music");
                    } else {
                        music::stop_music_track();

                        DEB_INFO("Disabled music");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::MenuItem("Save On Exit", nullptr, &data.options.save_on_exit)) {
                if (data.options.save_on_exit) {
                    DEB_INFO("The game will be saved on exit");
                } else {
                    DEB_INFO("The game will not be saved on exit");
                }
            }
            if (ImGui::BeginMenu("Skybox")) {
                // Don't directly set options.skybox
                static int skybox = data.options.skybox;

                if (ImGui::RadioButton("None", &skybox, 0)) {
                    if (skybox != data.options.skybox) {
                        data.options.skybox = skybox;
                        set_skybox(app, scene, Skybox::None);

                        DEB_INFO("Skybox set to none");
                    }
                }
                if (ImGui::RadioButton("Field", &skybox, 1)) {
                    if (skybox != data.options.skybox) {
                        data.options.skybox = skybox;
                        set_skybox(app, scene, Skybox::Field);

                        DEB_INFO("Skybox set to field");
                    }
                }
                if (ImGui::RadioButton("Autumn", &skybox, 2)) {
                    if (skybox != data.options.skybox) {
                        data.options.skybox = skybox;
                        set_skybox(app, scene, Skybox::Autumn);

                        DEB_INFO("Skybox set to autumn");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::MenuItem("Show Information", nullptr, &show_info)) {
                if (show_info) {
                    DEB_INFO("Show information");
                } else {
                    DEB_INFO("Hide information");
                }
            }
            if (ImGui::BeginMenu("Camera Sensitivity")) {
                ImGui::PushItemWidth(100.0f);
                if (ImGui::SliderFloat("##", &data.options.sensitivity, 0.5f, 2.0f, "%.01f", ImGuiSliderFlags_Logarithmic)) {
                    scene->camera_controller.sensitivity = data.options.sensitivity;

                    DEB_INFO("Changed camera sensitivity to {}", scene->camera_controller.sensitivity);
                }
                ImGui::PopItemWidth();

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            if (ImGui::BeginMenu("User Interface")) {
                if (ImGui::MenuItem("Hide Timer", nullptr, &data.options.hide_timer)) {
                    auto& data = app->user_data<Data>();

                    if (data.options.hide_timer) {
                        app->gui_renderer->remove_widget(app->res.text["timer_text"_h]);

                        DEB_INFO("Hide timer");
                    } else {
                        app->gui_renderer->add_widget(app->res.text["timer_text"_h]);

                        DEB_INFO("Show timer");
                    }
                }

                ImGui::EndMenu();
                HOVERING_GUI()
            }
            // Don't directly set options.labeled_board
            static bool labeled_board = data.options.labeled_board;

            if (ImGui::MenuItem("Labeled Board", nullptr, &labeled_board)) {
                if (labeled_board && labeled_board != data.options.labeled_board) {
                    data.options.labeled_board = labeled_board;
                    set_board_paint_texture(app, scene);

                    DEB_INFO("Board paint texture set to labeled");
                }

                if (!labeled_board && labeled_board != data.options.labeled_board) {
                    data.options.labeled_board = labeled_board;
                    set_board_paint_texture(app, scene);

                    DEB_INFO("Board paint texture set to non-labeled");
                }
            }

            ImGui::EndMenu();
            HOVERING_GUI()
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About", nullptr, false)) {
                show_about = true;
            }
            if (ImGui::MenuItem("Log Information", nullptr, false)) {
                logging::log_general_information(logging::LogTarget::File);

                DEB_INFO("Logged OpenGL and dependencies information");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", info_file_path.c_str());
            }

            ImGui::EndMenu();
            HOVERING_GUI()
        }

        ImGui::EndMainMenuBar();
    }
}

template<typename S, typename B>
void ImGuiLayer<S, B>::draw_info() {
    ImGui::PushFont(app->user_data<Data>().imgui_info_font);

    const int flags = (
        ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBackground
        | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoMove
    );

    ImGui::Begin("Information", nullptr, flags);
    ImGui::Text("FPS: %.3f", app->get_fps());
    ImGui::Text("OpenGL: %s", gl::get_opengl_version());
    ImGui::Text("Renderer: %s", gl::get_renderer());
    ImGui::End();

    ImGui::PopFont();
}

template<typename S, typename B>
void ImGuiLayer<S, B>::draw_game_over() {
    ImGui::PushFont(app->user_data<Data>().imgui_windows_font);
    ImGui::OpenPopup("Game Over");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Game Over", nullptr, window_flags)) {
        HOVERING_GUI()

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

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
        snprintf(time_label, 64, "Time: %s", time);

        const float text_width = ImGui::CalcTextSize(time_label).x;
        ImGui::SetCursorPosX((window_width - text_width) * 0.5f);
        ImGui::Text("%s", time_label);

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            scene->board.phase = BoardPhase::None;
        }

        ImGui::Dummy(ImVec2(0.0f, 2.0f));

        ImGui::EndPopup();
        HOVERING_GUI()
    }
    ImGui::PopFont();
}

template<typename S, typename B>
void ImGuiLayer<S, B>::draw_about() {
    ImGui::PushFont(app->user_data<Data>().imgui_windows_font);
    ImGui::OpenPopup("About Nine Morris 3D");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("About Nine Morris 3D", nullptr, window_flags)) {
        HOVERING_GUI()

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        ImGui::Text("A 3D implementation of the board game nine men's morris");
        ImGui::Text("Version %u.%u.%u", app->data().version_major, app->data().version_minor, app->data().version_patch);
        ImGui::Separator();
        ImGui::Text("All programming by:");
        ImGui::Text(u8"Simon-Teodor Mărăcine - simonmara.dev@gmail.com");

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        const float window_width = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            show_about = false;
        }

        ImGui::EndPopup();
    }
    ImGui::PopFont();
}

template<typename S, typename B>
void ImGuiLayer<S, B>::draw_could_not_load_game() {
    ImGui::PushFont(app->user_data<Data>().imgui_windows_font);
    ImGui::OpenPopup("Error Loading Game");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("Error Loading Game", nullptr, window_flags)) {
        HOVERING_GUI()

        ImGui::Dummy(ImVec2(0.0f, 5.0f));

        ImGui::Text("Could not load last game.");
        ImGui::Text("The save game file is either missing or is corrupted.");
        ImGui::Separator();
        ImGui::Text("%s", save_game_file_path.c_str());

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        const float window_width = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            show_could_not_load_game = false;
        }

        ImGui::EndPopup();
    }
    ImGui::PopFont();
}

template<typename S, typename B>
void ImGuiLayer<S, B>::draw_no_last_game() {
    ImGui::PushFont(app->user_data<Data>().imgui_windows_font);
    ImGui::OpenPopup("No Last Game");

    const ImVec2 center = ImGui::GetMainViewport()->GetCenter();
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal("No Last Game", nullptr, window_flags)) {
        HOVERING_GUI()

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        ImGui::Text("There is no last game saved.");

        ImGui::Dummy(ImVec2(0.0f, 10.0f));

        const float window_width = ImGui::GetWindowSize().x;
        ImGui::SetCursorPosX((window_width - 150.0f) * 0.5f);
        if (ImGui::Button("Ok", ImVec2(150.0f, 0.0f))) {
            ImGui::CloseCurrentPopup();
            show_no_last_game = false;
        }

        ImGui::EndPopup();
    }
    ImGui::PopFont();
}

#ifdef NM3D_PLATFORM_DEBUG
template<typename S, typename B>
void ImGuiLayer<S, B>::draw_debug() {
    if (!show_about) {
        ImGui::Begin("Debug");
        ImGui::Text("FPS: %.3f", app->get_fps());
        ImGui::Text("White pieces: %u", scene->board.white_pieces_count);
        ImGui::Text("Black pieces: %u", scene->board.black_pieces_count);
        ImGui::Text("Not placed white pieces: %u", scene->board.not_placed_white_pieces_count);
        ImGui::Text("Not placed black pieces: %u", scene->board.not_placed_black_pieces_count);
        ImGui::Text("Can jump: %s, %s", scene->board.can_jump[0] ? "true" : "false", scene->board.can_jump[1] ? "true" : "false");
        ImGui::Text("Phase: %d", static_cast<int>(scene->board.phase));
        ImGui::Text("Turn: %s", scene->board.turn == BoardPlayer::White ? "white" : "black");
        ImGui::Text("Must take piece: %s", scene->board.must_take_piece ? "true" : "false");
        ImGui::Text("Turns without mills: %u", scene->board.turns_without_mills);
        ImGui::Text("Undo history size: %lu", scene->undo_redo_state.undo.size());
        ImGui::Text("Redo history size: %lu", scene->undo_redo_state.redo.size());
        ImGui::Text("Hovered ID: %.3f", static_cast<float>(app->renderer->get_hovered_id()));
        ImGui::Text("Clicked node: %lu", scene->board.clicked_node_index);
        ImGui::Text("Clicked piece: %lu", scene->board.clicked_piece_index);
        ImGui::Text("Selected piece: %lu", scene->board.selected_piece_index);
        ImGui::Text("Is player's turn: %s", scene->board.is_players_turn ? "true" : "false");
        ImGui::Text("Next move: %s", scene->board.next_move ? "true" : "false");
        ImGui::Text("Game started: %s", scene->made_first_move ? "true" : "false");
        ImGui::End();

        {
            const float time = app->get_delta() * 1000.0f;
            frames[index] = time;

            if (index < FRAMES_SIZE) {
                index++;
                frames.push_back(time);
            } else {
                frames.push_back(time);
                frames.erase(frames.begin());
            }

            char text[32];
            snprintf(text, 32, "%.3f", time);

            ImGui::Begin("Frame Time");
            ImGui::PlotLines("time (ms)", frames.data(), FRAMES_SIZE, 0, text, 0.0f, 50.0f, ImVec2(200, 60));
            ImGui::End();
        }

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
        if (ImGui::SliderFloat3("Position", reinterpret_cast<float*>(&app->renderer->light.position), -30.0f, 30.0f)) {
            app->res.quad["light_bulb"_h]->position = app->renderer->light.position;
        }
        ImGui::SliderFloat3("Ambient color", reinterpret_cast<float*>(&app->renderer->light.ambient_color), 0.0f, 1.0f);
        ImGui::SliderFloat3("Diffuse color", reinterpret_cast<float*>(&app->renderer->light.diffuse_color), 0.0f, 1.0f);
        ImGui::SliderFloat3("Specular color", reinterpret_cast<float*>(&app->renderer->light.specular_color), 0.0f, 1.0f);
        ImGui::End();

        // If you recompile shaders, uniforms that are set only once need to be reuploaded
        /*
        ImGui::Begin("Shaders");  // TODO see what to do with this
        if (ImGui::Button("board_paint")) {
            // app->data.board_paint_shader->recompile();
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
        */

        const glm::vec3& position = scene->camera_controller.get_position();
        const glm::vec3& rotation = scene->camera_controller.get_rotation();

        ImGui::Begin("Camera");
        ImGui::Text("Position: %.3f, %.3f, %.3f", position.x, position.y, position.z);
        ImGui::Text("Pitch: %.3f", rotation.x);
        ImGui::Text("Yaw: %.3f", rotation.y);
        ImGui::Text("Angle around point: %.3f", scene->camera_controller.get_angle_around_point());
        ImGui::Text("Distance to point: %.3f", scene->camera_controller.get_distance_to_point());
        ImGui::End();

        ImGui::Begin("Light Space Matrix");
        ImGui::SliderFloat("Left", &app->renderer->light_space.left, -10.0f, 10.0f);
        ImGui::SliderFloat("Right", &app->renderer->light_space.right, -10.0f, 10.0f);
        ImGui::SliderFloat("Bottom", &app->renderer->light_space.bottom, -10.0f, 10.0f);
        ImGui::SliderFloat("Top", &app->renderer->light_space.top, -10.0f, 10.0f);
        ImGui::SliderFloat("Near", &app->renderer->light_space.lens_near, 0.1f, 2.0f);
        ImGui::SliderFloat("Far", &app->renderer->light_space.lens_far, 2.0f, 50.0f);
        ImGui::SliderFloat("Light divisor", &app->renderer->light_space.light_divisor, 1.0f, 10.0f);
        ImGui::End();
    }
}
#endif

template<typename S, typename B>
void ImGuiLayer<S, B>::draw_game_over_message(std::string_view message1, std::string_view message2) {
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
