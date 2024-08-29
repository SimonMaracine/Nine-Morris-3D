#include "ui.hpp"

#include <nine_morris_3d_engine/external/imgui.h++>

#include "global.hpp"
#include "enums.hpp"

void Ui::update(sm::Ctx& ctx) {
    main_menu_bar(ctx);
}

void Ui::main_menu_bar(sm::Ctx& ctx) {
    auto& g {ctx.global<Global>()};

    if (ImGui::BeginMainMenuBar()) {
        const bool enabled {true};

        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game", nullptr, false, enabled)) {
                // ctx->change_scene(ctx->get_current_scene()->get_id());

                // LOG_INFO("Restarted current game");
            }
            if (ImGui::MenuItem("Load Last Game", nullptr, false, enabled)) {
                // if (last_save_game_date == save_load::NO_LAST_GAME) {
                //     window = WindowImGui::ShowNoLastGame;
                // } else {
                //     load_game();
                // }
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", m_last_saved_game_date.c_str());
            }
            if (ImGui::MenuItem("Save Game", nullptr, false, enabled)) {
                // save_game();

                // time_t current;  // FIXME this time might be quite later
                // time(&current);
                // last_save_game_date = ctime(&current);
            }
            if (ImGui::BeginMenu("Game Mode", enabled)) {
                if (ImGui::RadioButton("Standard Game", &m_options.game_mode, static_cast<int>(GameMode::Standard))) {
                    // if (data.imgui_option.scene != data.options.scene) {
                    //     data.options.scene = data.imgui_option.scene;
                    //     ctx->change_scene("standard_game"_H);

                    //     LOG_INFO("Changed scene to standard game");
                    // }
                }
                if (ImGui::RadioButton("Jump Variant", &m_options.game_mode, static_cast<int>(GameMode::Jump))) {
                    // if (data.imgui_option.scene != data.options.scene) {
                    //     data.options.scene = data.imgui_option.scene;
                    //     ctx->change_scene("jump_variant"_H);

                    //     LOG_INFO("Changed scene to jump variant");
                    // }
                }
                if (ImGui::RadioButton("Jump Plus Variant", &m_options.game_mode, static_cast<int>(GameMode::JumpPlus))) {
                    // if (data.imgui_option.scene != data.options.scene) {
                    //     data.options.scene = data.imgui_option.scene;
                    //     ctx->change_scene("jump_plus_variant"_H);

                    //     LOG_INFO("Changed scene to jump plus variant");
                    // }
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Players", enabled)) {
                if (ImGui::BeginMenu("White")) {
                    if (ImGui::RadioButton("Human", &m_options.white_player, static_cast<int>(PlayerType::Human))) {
                        // game.white_player = GamePlayer::Human;

                        // if (game.state != GameState::Stop) {
                        //     game.reset_players();
                        // }

                        // LOG_INFO("Set white player to human");
                    }
                    if (ImGui::RadioButton("Computer", &m_options.white_player, static_cast<int>(PlayerType::Computer))) {
                        // game.white_player = GamePlayer::Computer;

                        // if (game.state != GameState::Stop) {
                        //     game.reset_players();
                        // }

                        // LOG_INFO("Set white player to computer");
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Black")) {
                    if (ImGui::RadioButton("Human", &m_options.black_player, static_cast<int>(PlayerType::Human))) {
                        // game.black_player = GamePlayer::Human;

                        // if (game.state != GameState::Stop) {
                        //     game.reset_players();
                        // }

                        // LOG_INFO("Set black player to human");
                    }
                    if (ImGui::RadioButton("Computer", &m_options.black_player, static_cast<int>(PlayerType::Computer))) {
                        // game.black_player = GamePlayer::Computer;

                        // if (game.state != GameState::Stop) {
                        //     game.reset_players();
                        // }

                        // LOG_INFO("Set black player to computer");
                    }

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Undo", nullptr, false, m_can_undo && enabled)) {
                // undo();

                // can_undo = get_undo_size() > 0;
                // can_redo = get_redo_size() > 0;
            }
            if (ImGui::MenuItem("Redo", nullptr, false, m_can_redo && enabled)) {
                // redo();

                // can_undo = get_undo_size() > 0;
                // can_redo = get_redo_size() > 0;
            }
            if (ImGui::MenuItem("Exit")) {
                ctx.running = false;
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::BeginMenu("Graphics")) {
                if (ImGui::MenuItem("VSync", nullptr, &m_options.vsync)) {
                    g.options.vsync = m_options.vsync;

                    ctx.set_window_vsync(g.options.vsync ? 1 : 0);
                }
                if (ImGui::MenuItem("Custom Cursor", nullptr, &m_options.custom_cursor)) {
                    // if (data.options.custom_cursor) {
                    //     if (get_board().must_take_piece) {
                    //         ctx->window->set_cursor("cross"_H);
                    //     } else {
                    //         ctx->window->set_cursor("arrow"_H);
                    //     }

                    //     LOG_INFO("Set custom cursor");
                    // } else {
                    //     ctx->window->set_cursor("null"_H);

                    //     LOG_INFO("Set default cursor");
                    // }
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Audio")) {
                if (ImGui::BeginMenu("Master Volume")) {
                    ImGui::PushItemWidth(100.0f);  // TODO DPI scale
                    if (ImGui::SliderFloat("##", &m_options.master_volume, 0.0f, 1.0f, "%.01f")) {
                        // ctx->snd->get_listener().set_gain(data.options.master_volume);

                        // LOG_INFO("Changed master volume to {}", data.options.master_volume);
                    }
                    ImGui::PopItemWidth();

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Music Volume")) {
                    ImGui::PushItemWidth(100.0f);
                    if (ImGui::SliderFloat("##", &m_options.music_volume, 0.0f, 1.0f, "%.01f")) {
                        // sm::music::set_music_gain(data.options.music_volume);

                        // LOG_INFO("Changed music volume to {}", data.options.music_volume);
                    }
                    ImGui::PopItemWidth();

                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("Enable Music", nullptr, &m_options.enable_music)) {
                    // if (data.options.enable_music) {
                    //     auto& data = ctx->data<Data>();

                    //     sm::music::play_music_track(data.current_music_track);

                    //     LOG_INFO("Enabled music");
                    // } else {
                    //     sm::music::stop_music_track();

                    //     LOG_INFO("Disabled music");
                    // }
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Computer AI")) {
                // window = WindowImGui::ShowAiSettings;
            }
            if (ImGui::MenuItem("Save On Exit", nullptr, &m_options.save_on_exit)) {
                // if (data.options.save_on_exit) {
                //     LOG_INFO("The game will be saved on exit");
                // } else {
                //     LOG_INFO("The game will not be saved on exit");
                // }
            }
            if (ImGui::BeginMenu("Skybox")) {
                if (m_loading_skybox) {
                    ImGui::RadioButton("None", false);
                    ImGui::RadioButton("Field", false);
                    ImGui::RadioButton("Autumn", false);
                } else {
                    if (ImGui::RadioButton("None", &m_options.skybox, static_cast<int>(Skybox::None))) {
                        // if (data.imgui_option.skybox != data.options.skybox) {
                        //     data.options.skybox = data.imgui_option.skybox;
                        //     set_skybox(Skybox::None);

                        //     LOG_INFO("Skybox set to None");
                        // }
                    }
                    if (ImGui::RadioButton("Field", &m_options.skybox, static_cast<int>(Skybox::Field))) {
                        // if (data.imgui_option.skybox != data.options.skybox) {
                        //     data.options.skybox = data.imgui_option.skybox;
                        //     set_skybox(Skybox::Field);

                        //     LOG_INFO("Skybox set to Field");
                        // }
                    }
                    if (ImGui::RadioButton("Autumn", &m_options.skybox, static_cast<int>(Skybox::Autumn))) {
                        // if (data.imgui_option.skybox != data.options.skybox) {
                        //     data.options.skybox = data.imgui_option.skybox;
                        //     set_skybox(Skybox::Autumn);

                        //     LOG_INFO("Skybox set to Autumn");
                        // }
                    }
                }

                ImGui::EndMenu();
            }
            ImGui::MenuItem("Show Information", nullptr, &m_show_information);
            if (ImGui::BeginMenu("Camera Sensitivity")) {
                ImGui::PushItemWidth(100.0f);
                if (ImGui::SliderFloat("##", &m_options.camera_sensitivity, 0.5f, 2.0f, "%.01f", ImGuiSliderFlags_Logarithmic)) {
                    // camera_controller.sensitivity = data.options.sensitivity;

                    // LOG_INFO("Changed camera sensitivity to {}", camera_controller.sensitivity);
                }
                ImGui::PopItemWidth();

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("User Interface")) {
                if (ImGui::MenuItem("Hide Timer", nullptr, &m_options.hide_timer)) {
                    // auto& data = ctx->data<Data>();

                    // if (data.options.hide_timer) {
                    //     scene_list.remove(objects.get<sm::gui::Text>("timer_text"_H));

                    //     LOG_INFO("Hide timer");
                    // } else {
                    //     scene_list.add(objects.get<sm::gui::Text>("timer_text"_H));

                    //     LOG_INFO("Show timer");
                    // }
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Labeled Board", nullptr, &m_options.labeled_board)) {
                // if (data.imgui_option.labeled_board != data.options.labeled_board) {
                //     data.options.labeled_board = data.imgui_option.labeled_board;
                //     set_board_paint_texture();

                //     if (data.imgui_option.labeled_board) {
                //         LOG_INFO("Board paint texture set to labeled");
                //     } else {
                //         LOG_INFO("Board paint texture set to non-labeled");
                //     }
                // }
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                // window = WindowImGui::ShowAbout;
            }
            if (ImGui::BeginMenu("Game Rules")) {
                if (ImGui::MenuItem("Standard Game")) {
                    // window = WindowImGui::ShowRulesStandardGame;
                }
                if (ImGui::MenuItem("Jump Variant")) {
                    // window = WindowImGui::ShowRulesJumpVariant;
                }
                if (ImGui::MenuItem("Jump Plus Variant")) {
                    // window = WindowImGui::ShowRulesJumpPlusVariant;
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Log Information")) {
                // sm::logging::log_general_information(sm::logging::LogTarget::File);

                // LOG_INFO("Logged OpenGL and dependencies information");
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", ctx.path_logs("some_file.txt").c_str());
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}
