#include "ui.hpp"

#include <utility>
#include <cassert>

#include <nine_morris_3d_engine/external/imgui.h++>
#include <nine_morris_3d_engine/external/glm.h++>
#include <nine_morris_3d_engine/external/resmanager.h++>

#include "scenes/game_scene.hpp"
#include "global.hpp"
#include "game_options.hpp"
#include "ver.hpp"

static bool resign_available(GameScene& game_scene) {
    return (
        game_scene.get_game_state() != GameState::Ready &&
        game_scene.get_game_state() != GameState::Over &&
        game_scene.get_game_options().game_type == GameTypeOnline
    );
}

static PlayerColor resign_player(GameScene& game_scene) {
    const GameOptions& options {game_scene.get_game_options()};

    PlayerColor color {};

    switch (options.game_type) {
        case GameTypeOnline:
            color = BoardObj::player_color_opponent(static_cast<PlayerColor>(options.online.remote_color));
            break;
        default:
            assert(false);
            break;
    }

    return color;
}

static bool offer_draw_available(GameScene& game_scene) {
    return (
        game_scene.get_game_state() != GameState::Ready &&
        game_scene.get_game_state() != GameState::Over &&
        game_scene.get_game_options().game_type == GameTypeOnline
    );
}

static bool accept_draw_offer_available(GameScene& game_scene) {
    return (
        game_scene.get_game_state() != GameState::Ready &&
        game_scene.get_game_state() != GameState::Over &&
        game_scene.get_game_options().game_type == GameTypeOnline &&
        game_scene.get_draw_offered_by_remote()
    );
}

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
    game_window(game_scene);

    switch (m_popup_window_queue.front()) {
        case PopupWindow::None:
            break;
        case PopupWindow::About:
            about_window();
            break;
        case PopupWindow::GameOver:
            game_over_window(game_scene);
            break;
        case PopupWindow::GameOptions:
            game_options_window(game_scene);
            break;
        case PopupWindow::EngineError:
            engine_error_window();
            break;
        case PopupWindow::ConnectionError:
            connection_error_window();
            break;
        case PopupWindow::RulesNineMensMorris:
            rules_nine_mens_morris_window();
            break;
    }
}

void Ui::push_popup_window(PopupWindow window) {
    m_popup_window_queue.push_back(window);
}

void Ui::main_menu_bar(sm::Ctx& ctx, GameScene& game_scene) {
    auto& g {ctx.global<Global>()};

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game")) {
                game_scene.reset();
            }
            if (ImGui::MenuItem("Resign", nullptr, nullptr, resign_available(game_scene))) {
                game_scene.resign(resign_player(game_scene));
            }
            if (ImGui::MenuItem("Accept Draw Offer", nullptr, nullptr, accept_draw_offer_available(game_scene))) {
                game_scene.accept_draw_offer();
            }
            if (ImGui::MenuItem("Offer Draw", nullptr, nullptr, offer_draw_available(game_scene))) {
                // TODO
            }
            if (ImGui::MenuItem("Game Options")) {
                m_popup_window_queue.push_back(PopupWindow::GameOptions);
            }
            if (ImGui::BeginMenu("Game Mode")) {
                if (ImGui::RadioButton("Nine Men's Morris", &m_options.game_mode, GameModeNineMensMorris)) {
                    if (std::exchange(g.options.game_mode, m_options.game_mode) != GameModeNineMensMorris) {
                        ctx.change_scene("nine_mens_morris"_H);
                    }
                }
                if (ImGui::RadioButton("Twelve Men's Morris", &m_options.game_mode, GameModeTwelveMensMorris)) {
                    if (std::exchange(g.options.game_mode, m_options.game_mode) != GameModeTwelveMensMorris) {
                        ctx.change_scene("twelve_mens_morris"_H);
                    }
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Exit")) {
                ctx.running = false;
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Options")) {
            if (ImGui::BeginMenu("Graphics")) {
                if (ImGui::BeginMenu("Scale")) {
                    if (ImGui::RadioButton("100%", &m_options.scale, Scale100)) {
                        if (std::exchange(g.options.scale, m_options.scale) != Scale100) {
                            set_scale_task(ctx, g.options.scale);
                        }
                    }
                    if (ImGui::RadioButton("200%", &m_options.scale, Scale200)) {
                        if (std::exchange(g.options.scale, m_options.scale) != Scale200) {
                            set_scale_task(ctx, g.options.scale);
                        }
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Anti-Aliasing")) {
                    if (ImGui::RadioButton("Off", &m_options.anti_aliasing, AntiAliasingOff)) {
                        if (std::exchange(g.options.anti_aliasing, m_options.anti_aliasing) != AntiAliasingOff) {
                            set_anti_aliasing_task(ctx, g.options.anti_aliasing);
                        }
                    }
                    if (ImGui::RadioButton("2X", &m_options.anti_aliasing, AntiAliasing2x)) {
                        if (std::exchange(g.options.anti_aliasing, m_options.anti_aliasing) != AntiAliasing2x) {
                            set_anti_aliasing_task(ctx, g.options.anti_aliasing);
                        }
                    }
                    if (ImGui::RadioButton("4X", &m_options.anti_aliasing, AntiAliasing4x)) {
                        if (std::exchange(g.options.anti_aliasing, m_options.anti_aliasing) != AntiAliasing4x) {
                            set_anti_aliasing_task(ctx, g.options.anti_aliasing);
                        }
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Anisotropic Filtering")) {
                    if (ImGui::RadioButton("Off", &m_options.anisotropic_filtering, AnisotropicFilteringOff)) {
                        if (std::exchange(g.options.anisotropic_filtering, m_options.anisotropic_filtering) != AnisotropicFilteringOff) {
                            set_anisotropic_filtering_task(ctx, game_scene);
                        }
                    }
                    if (ImGui::RadioButton("4X", &m_options.anisotropic_filtering, AnisotropicFiltering4x)) {
                        if (std::exchange(g.options.anisotropic_filtering, m_options.anisotropic_filtering) != AnisotropicFiltering4x) {
                            set_anisotropic_filtering_task(ctx, game_scene);
                        }
                    }
                    if (ImGui::RadioButton("8X", &m_options.anisotropic_filtering, AnisotropicFiltering8x)) {
                        if (std::exchange(g.options.anisotropic_filtering, m_options.anisotropic_filtering) != AnisotropicFiltering8x) {
                            set_anisotropic_filtering_task(ctx, game_scene);
                        }
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Texture Quality")) {
                    if (ImGui::RadioButton("Half", &m_options.texture_quality, TextureQualityHalf)) {
                        if (std::exchange(g.options.texture_quality, m_options.texture_quality) != TextureQualityHalf) {
                            game_scene.reload_and_set_textures();
                        }
                    }
                    if (ImGui::RadioButton("Full", &m_options.texture_quality, TextureQualityFull)) {
                        if (std::exchange(g.options.texture_quality, m_options.texture_quality) != TextureQualityFull) {
                            game_scene.reload_and_set_textures();
                        }
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::BeginMenu("Shadow Quality")) {
                    if (ImGui::RadioButton("Half", &m_options.shadow_quality, ShadowQualityHalf)) {
                        if (std::exchange(g.options.shadow_quality, m_options.shadow_quality) != ShadowQualityHalf) {
                            set_shadow_quality_task(ctx, g.options.shadow_quality);
                        }
                    }
                    if (ImGui::RadioButton("Full", &m_options.shadow_quality, ShadowQualityFull)) {
                        if (std::exchange(g.options.shadow_quality, m_options.shadow_quality) != ShadowQualityFull) {
                            set_shadow_quality_task(ctx, g.options.shadow_quality);
                        }
                    }

                    ImGui::EndMenu();
                }
                if (ImGui::MenuItem("VSync", nullptr, &m_options.vsync)) {
                    g.options.vsync = m_options.vsync;

                    ctx.set_window_vsync(g.options.vsync);
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
            if (ImGui::BeginMenu("Skybox")) {
                if (m_loading_skybox) {
                    ImGui::RadioButton("None", false);
                    ImGui::RadioButton("Field", false);
                    ImGui::RadioButton("Autumn", false);
                } else {
                    if (ImGui::RadioButton("None", &m_options.skybox, SkyboxNone)) {
                        if (std::exchange(g.options.skybox, m_options.skybox) != SkyboxNone) {
                            m_loading_skybox = true;
                            game_scene.reload_and_set_skybox();
                        }
                    }
                    if (ImGui::RadioButton("Field", &m_options.skybox, SkyboxField)) {
                        if (std::exchange(g.options.skybox, m_options.skybox) != SkyboxField) {
                            m_loading_skybox = true;
                            game_scene.reload_and_set_skybox();
                        }
                    }
                    if (ImGui::RadioButton("Autumn", &m_options.skybox, SkyboxAutumn)) {
                        if (std::exchange(g.options.skybox, m_options.skybox) != SkyboxAutumn) {
                            m_loading_skybox = true;
                            game_scene.reload_and_set_skybox();
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
            ImGui::MenuItem("Show Information", nullptr, &m_show_information);

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Engine")) {
            if (ImGui::BeginMenu("Name", static_cast<bool>(game_scene.get_engine()))) {
                ImGui::Text("%s", game_scene.get_engine()->get_name().c_str());

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Author", static_cast<bool>(game_scene.get_engine()))) {
                ImGui::Text("%s", game_scene.get_engine()->get_author().c_str());

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Restart", nullptr, nullptr, !game_scene.get_engine())) {
                game_scene.start_engine();
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("About")) {
                m_popup_window_queue.push_back(PopupWindow::About);
            }
            if (ImGui::BeginMenu("Game Rules")) {
                if (ImGui::MenuItem("Nine Men's Morris")) {
                    m_popup_window_queue.push_back(PopupWindow::RulesNineMensMorris);
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

void Ui::game_window(GameScene& game_scene) {
    if (ImGui::Begin("Game")) {
        if (game_scene.get_game_state() != GameState::Ready) {
            game_window_during_game(game_scene);
        } else {
            game_window_before_game(game_scene);
        }
    }

    ImGui::End();
}

void Ui::game_window_before_game(GameScene& game_scene) {
    // The engine may be down, so don't allow play
    ImGui::BeginDisabled(game_scene.get_game_options().game_type == GameTypeLocalHumanVsComputer && !game_scene.get_engine());

    if (ImGui::Button("Start Game")) {
        game_scene.get_game_state() = GameState::Start;
    }

    ImGui::EndDisabled();

    ImGui::Separator();

    switch (game_scene.get_game_options().game_type) {
        case GameTypeLocalHumanVsHuman:
            ImGui::TextWrapped("Local game between two human players");
            break;
        case GameTypeLocalHumanVsComputer:
            ImGui::TextWrapped("Local game between human and computer player");
            ImGui::TextWrapped(
                "Computer plays as %s",
                BoardObj::player_color_to_string(static_cast<PlayerColor>(game_scene.get_game_options().local_human_vs_computer.computer_color))
            );
            break;
        case GameTypeOnline:
            ImGui::TextWrapped("Online game between two human players");
            ImGui::TextWrapped(
                "Remote plays as %s",
                BoardObj::player_color_to_string(static_cast<PlayerColor>(game_scene.get_game_options().online.remote_color))
            );
            break;
    }

    const auto minutes {std::get<0>(Clock::split_time(game_scene.get_clock().get_white_time()))};

    if (minutes == 1) {
        ImGui::TextWrapped("%u minute", minutes);
    } else {
        ImGui::TextWrapped("%u minutes", minutes);
    }
}

void Ui::game_window_during_game(GameScene& game_scene) {
    {
        ImGui::Text("B");
        ImGui::SameLine();
        const auto [minutes, seconds, centiseconds] {Clock::split_time(game_scene.get_clock().get_black_time())};
        ImGui::Text("%u:%02u.%02u", minutes, seconds, centiseconds);
    }
    {
        ImGui::Text("W");
        ImGui::SameLine();
        const auto [minutes, seconds, centiseconds] {Clock::split_time(game_scene.get_clock().get_white_time())};
        ImGui::Text("%u:%02u.%02u", minutes, seconds, centiseconds);
    }

    ImGui::Separator();

    game_scene.get_move_list().update_window();
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
            case GameOver::Draw:
                message = "Draw!";
                break;
        }

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(message).x) * 0.5f);
        ImGui::Text("%s", message);

        const auto& reason {static_cast<const std::string&>(game_scene.get_board().get_game_over())};

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ImGui::CalcTextSize(reason.c_str()).x) * 0.5f);
        ImGui::Text("%s", reason.c_str());
    });
}

void Ui::game_options_window(GameScene& game_scene) {
    generic_window("Game Options", [&]() {
        GameOptions& options {game_scene.get_game_options()};

        ImGui::BeginDisabled(game_scene.get_game_state() != GameState::Ready);

        ImGui::RadioButton("Local", &options.game_type, GameTypeLocalHumanVsHuman);
        ImGui::RadioButton("Local vs Computer", &options.game_type, GameTypeLocalHumanVsComputer);
        ImGui::RadioButton("Online", &options.game_type, GameTypeOnline);

        ImGui::Separator();

        switch (options.game_type) {
            case GameTypeLocalHumanVsHuman:
                break;
            case GameTypeLocalHumanVsComputer:
                ImGui::RadioButton("white", &options.local_human_vs_computer.computer_color, PlayerColorWhite);
                ImGui::RadioButton("black", &options.local_human_vs_computer.computer_color, PlayerColorBlack);
                break;
            case GameTypeOnline:
                ImGui::RadioButton("white", &options.online.remote_color, PlayerColorWhite);
                ImGui::RadioButton("black", &options.online.remote_color, PlayerColorBlack);
                break;
        }

        ImGui::Separator();

        game_scene.time_control_options_window();

        ImGui::EndDisabled();
    });
}

void Ui::engine_error_window() {
    generic_window("Engine Error", []() {
        ImGui::Text("An error occurred with the engine. It has probably crashed.");
        ImGui::Text("You may want to restart it.");
    });
}

void Ui::connection_error_window() {
    generic_window("Connection Error", []() {
        ImGui::Text("An error occurred with the connection to the server.");
        ImGui::Text("You may want to reconnect.");
    });
}

void Ui::rules_nine_mens_morris_window() {
    const char* text {
R"(Each player has nine pieces, either black or white.
A player wins by reducing the opponent to two pieces, or by leaving them without a legal move.
When a player remains with three pieces, they can jump on the board.
A player may take a piece from a mill only if there are no other pieces available.
The game ends with a draw when fifty turns take place without any mill.
The game ends with a draw when the same position happens three times.)"
    };

    wrapped_text_window("Nine Men's Morris Rules", text);
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
            m_popup_window_queue.pop_front();

            on_ok();
        }

        ImGui::Dummy(ImVec2(0.0f, rem(0.5f)));

        ImGui::EndPopup();
    }
}

void Ui::set_scale(sm::Ctx& ctx, int scale) {
    create_font(ctx, scale);
    set_style();

    ImGuiStyle& style {ImGui::GetStyle()};
    style.ScaleAllSizes(static_cast<float>(scale));
}

void Ui::set_scale_task(sm::Ctx& ctx, int scale) {
    ctx.add_task([&ctx, scale]() {
        set_scale(ctx, scale);

        return sm::Task::Result::Done;
    });
}

void Ui::set_anti_aliasing_task(sm::Ctx& ctx, int samples) {
    ctx.add_task([&ctx, samples]() {
        ctx.set_renderer_samples(samples);

        return sm::Task::Result::Done;
    });
}

void Ui::set_shadow_quality_task(sm::Ctx& ctx, int size) {
    ctx.add_task([&ctx, size]() {
        ctx.set_renderer_shadow_map_size(size);

        return sm::Task::Result::Done;
    });
}

void Ui::set_anisotropic_filtering_task(sm::Ctx& ctx, GameScene& game_scene) {
    ctx.add_task([&game_scene]() {
        game_scene.reload_and_set_scene_textures();

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
