#include "ui.hpp"

#include <utility>

#include <nine_morris_3d_engine/external/imgui.h++>
#include <nine_morris_3d_engine/external/glm.h++>
#include <nine_morris_3d_engine/external/resmanager.h++>

#include "scenes/game_scene.hpp"
#include "global.hpp"
#include "constants.hpp"
#include "ver.hpp"

void Ui::initialize(sm::Ctx& ctx) {
    const auto& g {ctx.global<Global>()};

    m_options = g.options;

    set_scale(ctx, g.options.scale);

#ifndef SM_BUILD_DISTRIBUTION
    m_show_information = true;
#endif
}

void Ui::update(sm::Ctx& ctx, GameScene& game_scene) {
    main_menu_bar(ctx, game_scene);

    switch (m_current_popup_window) {
        case PopupWindow::None:
            break;
        case PopupWindow::About:
            about_window();
            break;
        case PopupWindow::GameOver:
            game_over_window(game_scene);
            break;
        case PopupWindow::RulesStandardGame:
            rules_standard_game_window();
            break;
        case PopupWindow::RulesJumpVariant:
            rules_jump_variant_window();
            break;
        case PopupWindow::RulesJumpPlusVariant:
            rules_jump_plus_variant_window();
            break;
        case PopupWindow::ComputerAi:
            computer_ai_window();
            break;
    }
}

void Ui::main_menu_bar(sm::Ctx& ctx, GameScene& game_scene) {
    auto& g {ctx.global<Global>()};

    if (ImGui::BeginMainMenuBar()) {
        const bool enabled {true};

        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game", nullptr, false, enabled)) {
                ctx.change_scene(dynamic_cast<sm::ApplicationScene&>(game_scene).get_id());
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
                    if (std::exchange(g.options.game_mode, m_options.game_mode) != static_cast<int>(GameMode::Standard)) {
                        ctx.change_scene("standard_game"_H);
                    }
                }
                if (ImGui::RadioButton("Jump Variant", &m_options.game_mode, static_cast<int>(GameMode::Jump))) {
                    if (std::exchange(g.options.game_mode, m_options.game_mode) != static_cast<int>(GameMode::Jump)) {
                        ctx.change_scene("jump_variant"_H);
                    }
                }
                if (ImGui::RadioButton("Jump Plus Variant", &m_options.game_mode, static_cast<int>(GameMode::JumpPlus))) {
                    if (std::exchange(g.options.game_mode, m_options.game_mode) != static_cast<int>(GameMode::JumpPlus)) {
                        ctx.change_scene("jump_plus_variant"_H);
                    }
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Players", enabled)) {
                if (ImGui::BeginMenu("White")) {
                    if (ImGui::RadioButton("Human", &m_options.white_player, static_cast<int>(PlayerType::Human))) {
                        if (std::exchange(g.options.black_player, m_options.white_player) != static_cast<int>(PlayerType::Human)) {

                        }
                        // game.white_player = GamePlayer::Human;

                        // if (game.state != GameState::Stop) {
                        //     game.reset_players();
                        // }

                        // LOG_INFO("Set white player to human");
                    }
                    if (ImGui::RadioButton("Computer", &m_options.white_player, static_cast<int>(PlayerType::Computer))) {
                        if (std::exchange(g.options.black_player, m_options.white_player) != static_cast<int>(PlayerType::Computer)) {

                        }
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
                        if (std::exchange(g.options.black_player, m_options.black_player) != static_cast<int>(PlayerType::Human)) {

                        }
                        // game.black_player = GamePlayer::Human;

                        // if (game.state != GameState::Stop) {
                        //     game.reset_players();
                        // }

                        // LOG_INFO("Set black player to human");
                    }
                    if (ImGui::RadioButton("Computer", &m_options.black_player, static_cast<int>(PlayerType::Computer))) {
                        if (std::exchange(g.options.black_player, m_options.black_player) != static_cast<int>(PlayerType::Computer)) {

                        }
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
                if (ImGui::BeginMenu("Scale")) {
                    if (ImGui::RadioButton("100%", &m_options.scale, static_cast<int>(Scale::_100))) {
                        if (std::exchange(g.options.scale, m_options.scale) != static_cast<int>(Scale::_100)) {
                            set_scale_task(ctx, g.options.scale);
                        }
                    }
                    if (ImGui::RadioButton("200%", &m_options.scale, static_cast<int>(Scale::_200))) {
                        if (std::exchange(g.options.scale, m_options.scale) != static_cast<int>(Scale::_200)) {
                            set_scale_task(ctx, g.options.scale);
                        }
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Anti-Aliasing")) {
                    if (ImGui::RadioButton("Off", &m_options.anti_aliasing, static_cast<int>(AntiAliasing::Off))) {
                        if (std::exchange(g.options.anti_aliasing, m_options.anti_aliasing) != static_cast<int>(AntiAliasing::Off)) {
                            set_anti_aliasing_task(ctx, g.options.anti_aliasing);
                        }
                    }
                    if (ImGui::RadioButton("2X", &m_options.anti_aliasing, static_cast<int>(AntiAliasing::_2x))) {
                        if (std::exchange(g.options.anti_aliasing, m_options.anti_aliasing) != static_cast<int>(AntiAliasing::_2x)) {
                            set_anti_aliasing_task(ctx, g.options.anti_aliasing);
                        }
                    }
                    if (ImGui::RadioButton("4X", &m_options.anti_aliasing, static_cast<int>(AntiAliasing::_4x))) {
                        if (std::exchange(g.options.anti_aliasing, m_options.anti_aliasing) != static_cast<int>(AntiAliasing::_4x)) {
                            set_anti_aliasing_task(ctx, g.options.anti_aliasing);
                        }
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Anisotropic Filtering")) {
                    if (ImGui::RadioButton("Off", &m_options.anisotropic_filtering, static_cast<int>(AnisotropicFiltering::Off))) {
                        if (std::exchange(g.options.anisotropic_filtering, m_options.anisotropic_filtering) != static_cast<int>(AnisotropicFiltering::Off)) {
                            // TODO
                        }
                    }
                    if (ImGui::RadioButton("4X", &m_options.anisotropic_filtering, static_cast<int>(AnisotropicFiltering::_4x))) {
                        if (std::exchange(g.options.anisotropic_filtering, m_options.anisotropic_filtering) != static_cast<int>(AnisotropicFiltering::_4x)) {
                            // TODO
                        }
                    }
                    if (ImGui::RadioButton("8X", &m_options.anisotropic_filtering, static_cast<int>(AnisotropicFiltering::_8x))) {
                        if (std::exchange(g.options.anisotropic_filtering, m_options.anisotropic_filtering) != static_cast<int>(AnisotropicFiltering::_8x)) {
                            // TODO
                        }
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Texture Quality")) {
                    if (ImGui::RadioButton("Half", &m_options.texture_quality, static_cast<int>(TextureQuality::Half))) {
                        if (std::exchange(g.options.texture_quality, m_options.texture_quality) != static_cast<int>(TextureQuality::Half)) {
                            // TODO
                        }
                    }
                    if (ImGui::RadioButton("Full", &m_options.texture_quality, static_cast<int>(TextureQuality::Full))) {
                        if (std::exchange(g.options.texture_quality, m_options.texture_quality) != static_cast<int>(TextureQuality::Full)) {
                            // TODO
                        }
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Shadow Quality")) {
                    if (ImGui::RadioButton("Half", &m_options.shadow_quality, static_cast<int>(ShadowQuality::Half))) {
                        if (std::exchange(g.options.shadow_quality, m_options.shadow_quality) != static_cast<int>(ShadowQuality::Half)) {
                            set_shadow_quality_task(ctx, g.options.shadow_quality);
                        }
                    }
                    if (ImGui::RadioButton("Full", &m_options.shadow_quality, static_cast<int>(ShadowQuality::Full))) {
                        if (std::exchange(g.options.shadow_quality, m_options.shadow_quality) != static_cast<int>(ShadowQuality::Full)) {
                            set_shadow_quality_task(ctx, g.options.shadow_quality);
                        }
                    }

                    ImGui::EndMenu();
                }
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
                    ImGui::PushItemWidth(rem(5.0f));
                    if (ImGui::SliderFloat("##", &m_options.master_volume, 0.0f, 1.0f, "%.01f")) {
                        // ctx->snd->get_listener().set_gain(data.options.master_volume);

                        // LOG_INFO("Changed master volume to {}", data.options.master_volume);
                    }
                    ImGui::PopItemWidth();

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Music Volume")) {
                    ImGui::PushItemWidth(rem(5.0f));
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
                m_current_popup_window = PopupWindow::ComputerAi;
            }
            if (ImGui::BeginMenu("Skybox")) {
                if (m_loading_skybox) {
                    ImGui::RadioButton("None", false);
                    ImGui::RadioButton("Field", false);
                    ImGui::RadioButton("Autumn", false);
                } else {
                    if (ImGui::RadioButton("None", &m_options.skybox, static_cast<int>(Skybox::None))) {
                        if (std::exchange(g.options.skybox, m_options.skybox) != static_cast<int>(Skybox::None)) {
                            m_loading_skybox = true;
                            game_scene.load_and_set_skybox();
                        }
                    }
                    if (ImGui::RadioButton("Field", &m_options.skybox, static_cast<int>(Skybox::Field))) {
                        if (std::exchange(g.options.skybox, m_options.skybox) != static_cast<int>(Skybox::Field)) {
                            m_loading_skybox = true;
                            game_scene.load_and_set_skybox();
                        }
                    }
                    if (ImGui::RadioButton("Autumn", &m_options.skybox, static_cast<int>(Skybox::Autumn))) {
                        if (std::exchange(g.options.skybox, m_options.skybox) != static_cast<int>(Skybox::Autumn)) {
                            m_loading_skybox = true;
                            game_scene.load_and_set_skybox();
                        }
                    }
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Camera Sensitivity")) {
                ImGui::PushItemWidth(rem(5.0f));
                if (ImGui::SliderFloat("##", &m_options.camera_sensitivity, 0.5f, 2.0f, "%.01f", ImGuiSliderFlags_Logarithmic)) {
                    g.options.camera_sensitivity = m_options.camera_sensitivity;

                    game_scene.get_camera_controller().sensitivity = g.options.camera_sensitivity;;
                }
                ImGui::PopItemWidth();

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("User Interface")) {
                if (ImGui::MenuItem("Hide Timer", nullptr, &m_options.hide_timer)) {
                    g.options.hide_timer = m_options.hide_timer;
                }
                if (ImGui::MenuItem("Hide Turn Indicator", nullptr, &m_options.hide_turn_indicator)) {
                    g.options.hide_turn_indicator = m_options.hide_turn_indicator;
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Labeled Board", nullptr, &m_options.labeled_board)) {
                g.options.labeled_board = m_options.labeled_board;

                game_scene.load_and_set_board_paint_texture();
            }
            if (ImGui::MenuItem("Save On Exit", nullptr, &m_options.save_on_exit)) {
                g.options.save_on_exit = m_options.save_on_exit;
            }
            ImGui::MenuItem("Show Information", nullptr, &m_show_information);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                m_current_popup_window = PopupWindow::About;
            }
            if (ImGui::BeginMenu("Game Rules")) {
                if (ImGui::MenuItem("Standard Game")) {
                    m_current_popup_window = PopupWindow::RulesStandardGame;
                }
                if (ImGui::MenuItem("Jump Variant")) {
                    m_current_popup_window = PopupWindow::RulesJumpVariant;
                }
                if (ImGui::MenuItem("Jump Plus Variant")) {
                    m_current_popup_window = PopupWindow::RulesJumpPlusVariant;
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Log Information")) {
                sm::utils::write_file(ctx.path_logs("information.txt"), ctx.get_information(), true);
            }
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", ctx.path_logs("information.txt").string().c_str());
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Ui::about_window() {
    generic_window("About Nine Morris 3D", []() {
        ImGui::Text("A 3D implementation of the board game nine men's morris");
        ImGui::Text("Version %u.%u.%u", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
        ImGui::Separator();
        ImGui::Text("All programming by:");
        ImGui::Text("Simon - simonmara.dev@gmail.com");
    });
}

void Ui::game_over_window(GameScene& game_scene) {
    generic_window("Game Over", [&]() {
        const char* message {};

        switch (game_scene.get_board().get_game_over()) {
            case GameOver::None:
                assert(false);
                break;
            case GameOver::WinnerWhite:
                message = "White player wins!";
                break;
            case GameOver::WinnerBlack:
                message = "Black player wins!";
                break;
            case GameOver::TieBetweenBothPlayers:
                message = "Tie between both players!";
                break;
        }

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(message).x) * 0.5f);
        ImGui::Text("%s", message);

        const auto reason {static_cast<std::string>(game_scene.get_board().get_game_over())};

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(reason.c_str()).x) * 0.5f);
        ImGui::Text("%s", reason.c_str());
    });
}

void Ui::rules_standard_game_window() {
    const char* text {
R"(Each player has nine pieces, either black or white.
A player wins by reducing the opponent to two pieces, or by leaving them without a legal move.
When a player remains with three pieces, they can jump on the board.
A player may take a piece from a mill only if there are no other pieces available.
The game ends with a tie when fifty turns take place without any mill.
The game ends with a tie when the exact same position shows up for the third time.)"
    };

    wrapped_text_window("Standard Game Rules", text);
}

void Ui::rules_jump_variant_window() {
    const char* text {
R"(Each player has only three pieces and can jump anywhere on the board.
The first player to form a mill wins.
The game ends with a tie when fifty turns take place without any mill.
The game ends with a tie when the exact same position shows up for the third time.)"
    };

    wrapped_text_window("Jump Variant", text);
}

void Ui::rules_jump_plus_variant_window() {
    const char* text {
R"(Same rules as the jump variant, but each player has six pieces instead of three.)"
    };

    wrapped_text_window("Jump Plus Variant Rules", text);
}

void Ui::computer_ai_window() {
    generic_window("Computer AI", []() {

    });
}

void Ui::wrapped_text_window(const char* title, const char* text) {
    const auto viewport_size {ImGui::GetMainViewport()->WorkSize};
    const bool wrapped {viewport_size.x < ImGui::CalcTextSize(text).x + ImGui::GetStyle().WindowPadding.x * 2.0f};

    generic_window(title, [=]() {
        if (wrapped) {
            ImGui::TextWrapped("%s", text);  // FIXME
        } else {
            ImGui::Text("%s", text);
        }
    });
}

void Ui::generic_window(const char* title, std::function<void()>&& contents, std::function<void()>&& on_ok) {
    ImGui::OpenPopup(title);

    const ImVec2 center {ImGui::GetMainViewport()->GetWorkCenter()};
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));

    const auto flags {ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse};

    if (ImGui::BeginPopupModal(title, nullptr, flags)) {
        contents();

        ImGui::Dummy(ImVec2(rem(8.0f), rem(0.5f)));

        const float ok_button_width {rem(7.0f)};
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ok_button_width) * 0.5f);

        if (ImGui::Button("Ok", ImVec2(ok_button_width, 0.0f))) {
            ImGui::CloseCurrentPopup();
            m_current_popup_window = PopupWindow::None;

            on_ok();
        }

        ImGui::Dummy(ImVec2(0.0f, rem(0.5f)));

        ImGui::EndPopup();
    }
}

void Ui::set_scale_task(sm::Ctx& ctx, int scale) {
    ctx.add_task([this, &ctx, scale](const sm::Task&, void*) {
        set_scale(ctx, scale);

        return sm::Task::Result::Done;
    });
}

void Ui::set_scale(sm::Ctx& ctx, int scale) {
    create_font(ctx, scale);
    set_style();

    ImGuiStyle& style {ImGui::GetStyle()};
    style.ScaleAllSizes(static_cast<float>(scale));
}

void Ui::set_anti_aliasing_task(sm::Ctx& ctx, int samples) {
    ctx.add_task([this, &ctx, samples](const sm::Task&, void*) {
        ctx.set_renderer_samples(samples);

        return sm::Task::Result::Done;
    });
}

void Ui::set_shadow_quality_task(sm::Ctx& ctx, int size) {
    ctx.add_task([this, &ctx, size](const sm::Task&, void*) {
        ctx.set_renderer_shadow_map_size(size);

        return sm::Task::Result::Done;
    });
}

void Ui::create_font(sm::Ctx& ctx, int scale) {
    static constexpr float BASE_FONT_SIZE {22.0f};

    const float font_size {glm::floor(BASE_FONT_SIZE * static_cast<float>(scale))};

    ImGuiIO& io {ImGui::GetIO()};

    // This is very needed :P
    io.Fonts->Clear();

    ImFontGlyphRangesBuilder builder;
    builder.AddRanges(io.Fonts->GetGlyphRangesDefault());
    builder.AddText(reinterpret_cast<const char*>(u8"ă"));
    ImVector<ImWchar> ranges;
    builder.BuildRanges(&ranges);

    const auto font {io.Fonts->AddFontFromFileTTF(
        ctx.path_assets("fonts/OpenSans/OpenSans-Semibold.ttf").string().c_str(),
        font_size,
        nullptr,
        ranges.Data
    )};

    io.FontDefault = font;
    io.Fonts->Build();

    // This was also not very documented :P
    ctx.invalidate_dear_imgui_texture();
}

void Ui::set_style() {
    ImGuiStyle& style {ImGui::GetStyle()};

    style = {};

    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.WindowRounding = 10.0f;
    style.ChildRounding = 10.0f;
    style.FrameRounding = 10.0f;
    style.PopupRounding = 10.0f;
    style.ScrollbarRounding = 10.0f;
    style.GrabRounding = 10.0f;
    style.TabRounding = 10.0f;
    style.DisplayWindowPadding = ImVec2(20.0f, 20.0f);
    style.DisplaySafeAreaPadding = ImVec2(4.0f, 4.0f);

    // TODO other?
}

float Ui::rem(float size) {
    return ImGui::GetFontSize() * size;
}
