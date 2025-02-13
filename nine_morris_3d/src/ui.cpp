#include "ui.hpp"

#include <utility>
#include <algorithm>
#include <iterator>
#include <cstring>
#include <cassert>

#include <nine_morris_3d_engine/external/imgui.h++>
#include <nine_morris_3d_engine/external/glm.h++>
#include <nine_morris_3d_engine/external/resmanager.h++>

#include "scenes/game_scene.hpp"
#include "global.hpp"
#include "game_options.hpp"
#include "ver.hpp"
#include "window_size.hpp"

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
    game_window(ctx, game_scene);

    // This way the session window is visible only when the user is in a session
    if (game_scene.get_game_session()) {
        game_scene.get_game_session()->session_window(ctx, game_scene);
    }

    if (!m_modal_window_queue.empty()) {
        const auto& [modal_window, string] {m_modal_window_queue.front()};

        switch (modal_window) {
            case ModalWindowNone:
                break;
            case ModalWindowGeneralPlay:
                general_play_window();
                break;
            case ModalWindowOnlinePlay:
                online_play_window();
                break;
            case ModalWindowAbout:
                about_window();
                break;
            case ModalWindowGameOver:
                game_over_window(game_scene);
                break;
            case ModalWindowGameOptions:
                game_options_window(ctx, game_scene);
                break;
            case ModalWindowEngineError:
                engine_error_window();
                break;
            case ModalWindowConnectionError:
                connection_error_window();
                break;
            case ModalWindowServerRejection:
                server_rejection_window(string);
                break;
            case ModalWindowNewGameSessionError:
                new_game_session_error_window(string);
                break;
            case ModalWindowJoinGameSessionError:
                join_game_session_error_window(string);
                break;
            case ModalWindowWaitServerAcceptGameSession:
                wait_server_accept_game_session_window(game_scene);
                break;
            case ModalWindowWaitRemoteJoinGameSession:
                wait_remote_join_game_session_window(game_scene);
                break;
            case ModalWindowWaitServerAcceptJoinGameSession:
                wait_server_accept_join_game_session_window(game_scene);
                break;
            case ModalWindowWaitRemoteRematch:
                wait_remote_rematch_window(game_scene);
                break;
            case ModalWindowRulesNineMensMorris:
                rules_nine_mens_morris_window();
                break;
            case ModalWindowRulesTwelveMensMorris:
                rules_twelve_mens_morris_window();
                break;
        }
    }
}

void Ui::push_modal_window(ModalWindow window, const std::string& string) {
    m_modal_window_queue.emplace_back(window, string);
}

void Ui::clear_modal_window() {
    m_modal_window_queue.clear();
}

void Ui::clear_modal_window(unsigned int windows) {
    m_modal_window_queue.erase(std::remove_if(m_modal_window_queue.begin(), m_modal_window_queue.end(), [=](auto window) {
        return windows & static_cast<unsigned int>(window.first);
    }), m_modal_window_queue.end());
}

ModalWindow Ui::get_modal_window() const {
    if (!m_modal_window_queue.empty()) {
        return m_modal_window_queue.front().first;
    }

    return ModalWindowNone;
}

float Ui::rem(float size) {
    return ImGui::GetFontSize() * size;
}

void Ui::main_menu_bar(sm::Ctx& ctx, GameScene& game_scene) {
    auto& g {ctx.global<Global>()};

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("Game")) {
            if (ImGui::MenuItem("New Game")) {
                // Must resign first
                if (resign_available(game_scene)) {
                    // Cannot display the resign game over popup
                    game_scene.client_resign();
                }

                if (game_scene.get_game_session()) {
                    game_scene.client_leave_game_session();
                }

                game_scene.reset();
            }
            if (ImGui::MenuItem("Resign", nullptr, nullptr, resign_available(game_scene))) {
                game_scene.resign(resign_player(game_scene));
                game_scene.client_resign();
            }
            if (ImGui::MenuItem("Accept Draw", nullptr, nullptr, accept_draw_available(game_scene))) {
                game_scene.accept_draw();
                game_scene.client_accept_draw();
            }
            if (ImGui::MenuItem("Offer Draw", nullptr, nullptr, offer_draw_available(game_scene))) {
                game_scene.client_offer_draw();
            }
            if (ImGui::MenuItem("Game Options")) {
                push_modal_window(ModalWindowGameOptions);
            }
            if (ImGui::BeginMenu("Game Mode")) {
                // Scene on_stop takes care of resigning and leaving the session
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

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Audio")) {
                if (ImGui::BeginMenu("Volume")) {
                    ImGui::PushItemWidth(rem(5.0f));
                    if (ImGui::SliderFloat("##", &m_options.audio_volume, 0.0f, 1.0f, "%.01f")) {
                        g.options.audio_volume = m_options.audio_volume;

                        sm::Ctx::set_audio_volume(g.options.audio_volume);
                    }
                    ImGui::PopItemWidth();

                    ImGui::EndMenu();
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Skybox")) {
                if (m_loading_skybox) {
                    ImGui::RadioButton("None", false);
                    ImGui::RadioButton("Field", false);
                    ImGui::RadioButton("Autumn", false);
                    ImGui::RadioButton("Summer", false);
                    ImGui::RadioButton("Night", false);
                    ImGui::RadioButton("Sunset", false);
                    ImGui::RadioButton("Sky", false);
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
                    if (ImGui::RadioButton("Summer", &m_options.skybox, SkyboxSummer)) {
                        if (std::exchange(g.options.skybox, m_options.skybox) != SkyboxSummer) {
                            m_loading_skybox = true;
                            game_scene.reload_and_set_skybox();
                        }
                    }
                    if (ImGui::RadioButton("Night", &m_options.skybox, SkyboxNight)) {
                        if (std::exchange(g.options.skybox, m_options.skybox) != SkyboxNight) {
                            m_loading_skybox = true;
                            game_scene.reload_and_set_skybox();
                        }
                    }
                    if (ImGui::RadioButton("Sunset", &m_options.skybox, SkyboxSunset)) {
                        if (std::exchange(g.options.skybox, m_options.skybox) != SkyboxSunset) {
                            m_loading_skybox = true;
                            game_scene.reload_and_set_skybox();
                        }
                    }
                    if (ImGui::RadioButton("Sky", &m_options.skybox, SkyboxSky)) {
                        if (std::exchange(g.options.skybox, m_options.skybox) != SkyboxSky) {
                            m_loading_skybox = true;
                            game_scene.reload_and_set_skybox();
                        }
                    }
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Camera Sensitivity")) {
                ImGui::PushItemWidth(rem(5.0f));
                if (ImGui::SliderFloat("##", &m_options.camera_sensitivity, 0.5f, 2.0f, "%.01f")) {
                    g.options.camera_sensitivity = m_options.camera_sensitivity;

                    game_scene.get_camera_controller()->sensitivity = g.options.camera_sensitivity;;
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
        if (ImGui::BeginMenu("Server")) {
            if (ImGui::BeginMenu("Status")) {
                const char* status {};

                switch (g.client.get_connection_state()) {
                    case ConnectionState::Disconnected:
                        status = "Disconnected";
                        break;
                    case ConnectionState::Connecting:
                        status = "Connecting";
                        break;
                    case ConnectionState::Connected:
                        status = "Connected";
                        break;
                }

                ImGui::Text("%s", status);

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Session")) {
                if (ImGui::BeginMenu("Code", static_cast<bool>(game_scene.get_game_session()))) {
                    ImGui::Text("%.5u", game_scene.get_game_session()->get_session_id());

                    ImGui::EndMenu();
                }

                ImGui::PushItemWidth(rem(6.0f));
                if (ImGui::InputText("Name", m_options.name, sizeof(m_options.name))) {
                    std::memcpy(g.options.name, m_options.name, sizeof(m_options.name));
                }
                ImGui::PopItemWidth();

                if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                    ImGui::SetTooltip("Your name appears to other players.");
                }

                ImGui::EndMenu();
            }
            if (ImGui::BeginMenu("Address")) {
                if (ImGui::Checkbox("Default", &m_options.default_address_port)) {
                    g.options.default_address_port = m_options.default_address_port;
                }
                ImGui::PushItemWidth(rem(5.0f));
                if (g.options.default_address_port) {
                    ImGui::BeginDisabled();
                    ImGui::InputText("Address", m_options.address, sizeof(m_options.address));
                    ImGui::InputText("Port", m_options.port, sizeof(m_options.port));
                    ImGui::EndDisabled();
                } else {
                    if (ImGui::InputText("Address", m_options.address, sizeof(m_options.address))) {
                        std::memcpy(g.options.address, m_options.address, sizeof(m_options.address));
                    }

                    if (ImGui::InputText("Port", m_options.port, sizeof(m_options.port))) {
                        std::memcpy(g.options.port, m_options.port, sizeof(m_options.port));
                    }
                }
                ImGui::PopItemWidth();

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("Reconnect")) {
                if (g.options.default_address_port) {
                    game_scene.connect();
                } else {
                    game_scene.connect(g.options.address, g.options.port);
                }
            }

            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Help")) {
            if (ImGui::MenuItem("General Play")) {
                push_modal_window(ModalWindowGeneralPlay);
            }
            if (ImGui::MenuItem("Online Play")) {
                push_modal_window(ModalWindowOnlinePlay);
            }
            if (ImGui::BeginMenu("Game Rules")) {
                if (ImGui::MenuItem("Nine Men's Morris")) {
                    push_modal_window(ModalWindowRulesNineMensMorris);
                }
                if (ImGui::MenuItem("Twelve Men's Morris")) {
                    push_modal_window(ModalWindowRulesTwelveMensMorris);
                }

                ImGui::EndMenu();
            }
            if (ImGui::MenuItem("About")) {
                push_modal_window(ModalWindowAbout);
            }
            if (ImGui::MenuItem("Log Information")) {
                sm::utils::write_file(ctx.path_logs("nine_morris_3d.txt"), ctx.get_information(), true);
            }
            if (ImGui::IsItemHovered(ImGuiHoveredFlags_DelayNormal)) {
                ImGui::SetTooltip("%s", ctx.path_logs("nine_morris_3d.txt").string().c_str());
            }

            ImGui::EndMenu();
        }

        ImGui::EndMainMenuBar();
    }
}

void Ui::game_window(sm::Ctx& ctx, GameScene& game_scene) {
    const float width {sm::utils::map(
        static_cast<float>(ctx.get_window_width()),
        static_cast<float>(MIN_WIDTH),
        static_cast<float>(MAX_WIDTH),
        rem(11.0f),
        rem(15.0f)
    )};

    const float height_offset {sm::utils::map(
        static_cast<float>(ctx.get_window_height()),
        static_cast<float>(MIN_HEIGHT),
        static_cast<float>(MAX_HEIGHT),
        rem(5.0f),
        rem(15.0f)
    )};

    const float right_offset {sm::utils::map(
        static_cast<float>(ctx.get_window_width()),
        static_cast<float>(MIN_WIDTH),
        static_cast<float>(MAX_WIDTH),
        rem(1.0f),
        rem(2.0f)
    )};

    const float height {static_cast<float>(ctx.get_window_height()) - height_offset};

    ImGui::SetNextWindowPos(
        ImVec2(static_cast<float>(ctx.get_window_width()) - width - right_offset, static_cast<float>(ctx.get_window_height() / 2)),
        ImGuiCond_Always,
        ImVec2(0.0f, 0.5f)
    );

    ImGui::SetNextWindowSize(ImVec2(width, height), ImGuiCond_Always);

    if (ImGui::Begin("Game", nullptr, ImGuiWindowFlags_NoDecoration)) {
        if (game_scene.get_game_state() != GameState::Ready) {
            during_game_window(game_scene);
        } else {
            before_game_window(ctx, game_scene);
        }
    }

    ImGui::End();
}

void Ui::before_game_window(sm::Ctx& ctx, GameScene& game_scene) {
    const auto& g {ctx.global<Global>()};

    switch (g.options.game_type) {
        case GameTypeLocalHumanVsHuman:
        case GameTypeLocalHumanVsComputer:
            before_game_local_window(ctx, game_scene);
            break;
        case GameTypeOnline:
            before_game_online_window(ctx, game_scene);
            break;
    }

    const auto minutes {std::get<0>(Clock::split_time(game_scene.get_clock().get_white_time()))};

    if (minutes == 1) {
        ImGui::TextWrapped("%u minute", minutes);
    } else {
        ImGui::TextWrapped("%u minutes", minutes);
    }
}

void Ui::before_game_local_window(sm::Ctx& ctx, GameScene& game_scene) {
    const auto& g {ctx.global<Global>()};

    // The engine may be down, so don't allow play
    ImGui::BeginDisabled(g.options.game_type == GameTypeLocalHumanVsComputer && !game_scene.get_engine());
    if (ImGui::Button("Start Game")) {
        game_scene.get_game_state() = GameState::Start;
    }
    ImGui::EndDisabled();

    ImGui::Dummy(ImVec2(0.0f, rem(0.1f)));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, rem(0.1f)));

    switch (g.options.game_type) {
        case GameTypeLocalHumanVsHuman:
            ImGui::TextWrapped("Local game between two human players");
            break;
        case GameTypeLocalHumanVsComputer:
            ImGui::TextWrapped("Local game between human and computer player");
            ImGui::TextWrapped(
                "Computer plays as %s",
                to_string(static_cast<PlayerColor>(game_scene.get_game_options().computer_color))
            );
            break;
        case GameTypeOnline:
            assert(false);
            break;
    }
}

void Ui::before_game_online_window(sm::Ctx& ctx, GameScene& game_scene) {
    const auto& g {ctx.global<Global>()};

    assert(g.options.game_type == GameTypeOnline);

    // The connection may be down, so don't allow play
    ImGui::BeginDisabled(g.client.get_connection_state() == ConnectionState::Disconnected);

    ImGui::BeginDisabled(static_cast<bool>(game_scene.get_game_session()));
    if (ImGui::Button("Start Game")) {
        game_scene.client_request_game_session();
    }
    ImGui::EndDisabled();

    // The user must enter 5 digits
    ImGui::BeginDisabled(!join_game_available(game_scene));
    if (ImGui::Button("Join Game")) {
        game_scene.client_request_join_game_session(m_session_id);
    }
    ImGui::EndDisabled();

    ImGui::SameLine();

    ImGui::PushItemWidth(rem(3.0f));
    if (ImGui::InputText("Code", m_session_id, sizeof(m_session_id), ImGuiInputTextFlags_EnterReturnsTrue)) {
        if (join_game_available(game_scene)) {
            game_scene.client_request_join_game_session(m_session_id);
        }
    }
    ImGui::PopItemWidth();

    ImGui::EndDisabled();

    ImGui::Dummy(ImVec2(0.0f, rem(0.1f)));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, rem(0.1f)));

    ImGui::TextWrapped("Online game between two human players");
    ImGui::TextWrapped(
        "Remote plays as %s",
        to_string(static_cast<PlayerColor>(game_scene.get_game_options().remote_color))
    );
}

void Ui::during_game_window(GameScene& game_scene) {
    {
        ImGui::Image(game_scene.get_icon_black()->get_id(), ImVec2(rem(1.0f), rem(1.0f)));
        ImGui::SameLine();
        const auto [minutes, seconds, centiseconds] {Clock::split_time(game_scene.get_clock().get_black_time())};
        ImGui::Text("%u:%02u.%02u", minutes, seconds, centiseconds);
    }
    {
        ImGui::Image(game_scene.get_icon_white()->get_id(), ImVec2(rem(1.0f), rem(1.0f)));
        ImGui::SameLine();
        const auto [minutes, seconds, centiseconds] {Clock::split_time(game_scene.get_clock().get_white_time())};
        ImGui::Text("%u:%02u.%02u", minutes, seconds, centiseconds);
    }

    ImGui::Dummy(ImVec2(0.0f, rem(0.1f)));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0.0f, rem(0.1f)));

    game_scene.get_move_list().moves_window();
}

void Ui::about_window() {
    modal_window_ok("About Nine Morris 3D", []() {
        ImGui::Text("A 3D implementation of the board game nine men's morris");
        ImGui::Text("Version %u.%u.%u", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);
        ImGui::Separator();
        ImGui::Text("All programming by:");
        ImGui::Text("Simon <simonmara.dev@gmail.com>");
    });
}

void Ui::general_play_window() {
    modal_window_ok_size_constraints(
        "Help General Play",
        []() {
            ImGui::SeparatorText("Starting A Game:");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("From the menu, select the type of game you want to play: Game Mode and Game Options.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("Start the game from the right side interface.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("To play another game, simply select New Game from the menu.");

            ImGui::Spacing();

            ImGui::SeparatorText("Game Controls:");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("Interact with the board and pieces by using the left mouse click.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("Move the camera by dragging the mouse with right click, or by using the keys: W, A, S, D, R, F.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("Reset the camera to the default position by pressing the space key.");
        },
        glm::vec2(MIN_WIDTH - rem(1.0f), 0.0f),
        glm::vec2(ImGui::GetMainViewport()->WorkSize.x - rem(0.5f), ImGui::GetMainViewport()->WorkSize.y - rem(0.5f))
    );
}

void Ui::online_play_window() {
    modal_window_ok_size_constraints(
        "Help Online Play",
        []() {
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped(
                "Playing online with somebody is very simple: One party has to create the game and send the code to the opponent"
                " and the other party has to simply enter the code and join the game."
            );
            ImGui::TextWrapped("Before playing, though, know these things:");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped(
                "Each player has a name that by default is empty. Configure your name from the menu by selecting"
                " Server then Session and then Name. Your name is visible to the other player during the game"
                " and it is purely for aesthetics."
            );
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("During the game, players may choose to resign, to offer or to accept a draw. They can do so from the menu.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped(
                "After a game, players may choose to play again by selecting Rematch. In the next game they"
                " will have swapped sides."
            );
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped(
                "Please refrain from cheating. Have fun! :)"
            );
        },
        glm::vec2(MIN_WIDTH - rem(1.0f), 0.0f),
        glm::vec2(ImGui::GetMainViewport()->WorkSize.x - rem(0.5f), ImGui::GetMainViewport()->WorkSize.y - rem(0.5f))
    );
}

void Ui::game_over_window(GameScene& game_scene) {
    modal_window_ok("Game Over", [&]() {
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

void Ui::game_options_window(sm::Ctx& ctx, GameScene& game_scene) {
    modal_window_ok_size(
        "Game Options",
        [&]() {
            auto& g {ctx.global<Global>()};
            GameOptions& game_options {game_scene.get_game_options()};

            ImGui::BeginDisabled(game_scene.get_game_state() != GameState::Ready);

            ImGui::SeparatorText("Game Type");
            if (ImGui::RadioButton("Local", &g.options.game_type, GameTypeLocalHumanVsHuman)) {
                game_scene.reset_camera_position();
            }
            ImGui::SameLine();
            if (ImGui::RadioButton("Local vs Computer", &g.options.game_type, GameTypeLocalHumanVsComputer)) {
                game_scene.reset_camera_position();
            }
            if (ImGui::RadioButton("Online", &g.options.game_type, GameTypeOnline)) {
                game_scene.reset_camera_position();
            }
            ImGui::Dummy(ImVec2(0.0f, rem(0.4f)));

            switch (g.options.game_type) {
                case GameTypeLocalHumanVsHuman:
                    break;
                case GameTypeLocalHumanVsComputer:
                    ImGui::SeparatorText("Computer Plays As");
                    if (ImGui::RadioButton("White", &game_options.computer_color, PlayerColorWhite)) {
                        game_scene.reset_camera_position();
                    }
                    ImGui::SameLine();
                    if (ImGui::RadioButton("Black", &game_options.computer_color, PlayerColorBlack)) {
                        game_scene.reset_camera_position();
                    }
                    ImGui::Dummy(ImVec2(0.0f, rem(0.4f)));
                    break;
                case GameTypeOnline:
                    ImGui::SeparatorText("Remote Plays As");
                    if (ImGui::RadioButton("White", &game_options.remote_color, PlayerColorWhite)) {
                        game_scene.reset_camera_position();
                    }
                    ImGui::SameLine();
                    if (ImGui::RadioButton("Black", &game_options.remote_color, PlayerColorBlack)) {
                        game_scene.reset_camera_position();
                    }
                    ImGui::Dummy(ImVec2(0.0f, rem(0.4f)));
                    break;
            }

            ImGui::SeparatorText("Time Control");
            game_scene.time_control_options_window();

            ImGui::EndDisabled();
        },
        glm::vec2(rem(15.0f), 0.0f)
    );
}

void Ui::engine_error_window() {
    modal_window_ok("Engine Error", []() {
        ImGui::Text("An error occurred with the engine. It has probably crashed.");
        ImGui::Text("You may want to restart it.");
    });
}

void Ui::connection_error_window() {
    modal_window_ok("Connection Error", []() {
        ImGui::Text("An error occurred with the connection to the server.");
        ImGui::Text("You may want to reconnect.");
    });
}

void Ui::server_rejection_window(const std::string& string) {
    modal_window_ok("Server Error", [&string]() {
        ImGui::Text("The server rejected the connection.");
        ImGui::Text("%s.", string.c_str());
    });
}

void Ui::new_game_session_error_window(const std::string& string) {
    modal_window_ok("New Online Game Error", [&string]() {
        ImGui::Text("Could not start a new online game.");
        ImGui::Text("%s.", string.c_str());
    });
}

void Ui::join_game_session_error_window(const std::string& string) {
    modal_window_ok("Join Online Game Error", [&string]() {
        ImGui::Text("Could not join the online game.");
        ImGui::Text("%s.", string.c_str());
    });
}

void Ui::wait_server_accept_game_session_window(GameScene&) {
    modal_window("Waiting For Server To Accept", []() {
        ImGui::Text("The server should reply any time soon.");

        return false;
    });
}

void Ui::wait_remote_join_game_session_window(GameScene& game_scene) {
    assert(game_scene.get_game_session());

    modal_window("Waiting For Player To Join", [&game_scene]() {
        ImGui::Text("An online game has been created.");
        ImGui::Text("Send your opponent the following code: %.5u", game_scene.get_game_session()->get_session_id());

        ImGui::Dummy(ImVec2(0.0f, rem(0.5f)));

        if (ImGui::Button("Cancel Game")) {
            // If the other user has already joined, it will still be notified by our forfeit
            game_scene.client_leave_game_session();

            return true;
        }

        return false;
    });
}

void Ui::wait_server_accept_join_game_session_window(GameScene&) {
    modal_window("Waiting For Server To Accept", []() {
        ImGui::Text("The server should reply any time soon.");

        return false;
    });
}

void Ui::wait_remote_rematch_window(GameScene& game_scene) {
    assert(game_scene.get_game_session());

    modal_window("Waiting For Player", [&game_scene]() {
        ImGui::Text("For another game to begin, the opponent has to accept a rematch as well.");
        ImGui::Text("Once the opponnent has accepted the rematch, it cannot be canceled.");

        ImGui::Dummy(ImVec2(0.0f, rem(0.5f)));

        if (ImGui::Button("Cancel Rematch")) {
            game_scene.client_cancel_rematch();
        }

        return false;
    });
}

void Ui::rules_nine_mens_morris_window() {
    modal_window_ok_size_constraints(
        "Nine Men's Morris Rules",
        []() {
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("Each player has nine pieces, either black or white.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("A player wins by reducing the opponent to two pieces, or by leaving them without a legal move.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("When a player remains with three pieces, they can jump on the board.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("A player may take a piece from a mill only if there are no other pieces available.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("The game ends with a draw when fifty turns are made without mills.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("The game ends with a draw when the same position happens three times.");
        },
        glm::vec2(MIN_WIDTH - rem(1.0f), 0.0f),
        glm::vec2(ImGui::GetMainViewport()->WorkSize.x - rem(0.5f), ImGui::GetMainViewport()->WorkSize.y - rem(0.5f))
    );
}

void Ui::rules_twelve_mens_morris_window() {
    modal_window_ok_size_constraints(
        "Twelve Men's Morris Rules",
        []() {
            ImGui::TextWrapped("Same rules apply as in nine men's morris, except the following:");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("Each player has twelve pieces instead of nine.");
            ImGui::Bullet();
            ImGui::SameLine();
            ImGui::TextWrapped("Players can make mills and move pieces along the diagonals.");
        },
        glm::vec2(MIN_WIDTH - rem(1.0f), 0.0f),
        glm::vec2(ImGui::GetMainViewport()->WorkSize.x - rem(0.5f), ImGui::GetMainViewport()->WorkSize.y - rem(0.5f))
    );
}

void Ui::modal_window(const char* title, std::function<bool()>&& contents) {
    ImGui::OpenPopup(title);

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetWorkCenter(), 0, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
        if (contents()) {
            ImGui::CloseCurrentPopup();
            m_modal_window_queue.pop_front();
        }

        ImGui::Dummy(ImVec2(0.0f, rem(0.5f)));

        ImGui::EndPopup();
    }
}

void Ui::modal_window_ok(const char* title, std::function<void()>&& contents) {
    ImGui::OpenPopup(title);

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetWorkCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
        contents();

        ImGui::Dummy(ImVec2(rem(8.0f), rem(0.5f)));

        const float ok_button_width {rem(6.0f)};
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ok_button_width) * 0.5f);

        if (ImGui::Button("Ok", ImVec2(ok_button_width, 0.0f))) {
            ImGui::CloseCurrentPopup();
            m_modal_window_queue.pop_front();
        }

        ImGui::Dummy(ImVec2(0.0f, rem(0.5f)));

        ImGui::EndPopup();
    }
}

void Ui::modal_window_ok_size(
    const char* title,
    std::function<void()>&& contents,
    glm::vec2 size
) {
    ImGui::OpenPopup(title);

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetWorkCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSize(ImVec2(size.x, size.y), ImGuiCond_Always);

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse)) {
        contents();

        ImGui::Dummy(ImVec2(rem(8.0f), rem(0.5f)));

        const float ok_button_width {rem(6.0f)};
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ok_button_width) * 0.5f);

        if (ImGui::Button("Ok", ImVec2(ok_button_width, 0.0f))) {
            ImGui::CloseCurrentPopup();
            m_modal_window_queue.pop_front();
        }

        ImGui::Dummy(ImVec2(0.0f, rem(0.5f)));

        ImGui::EndPopup();
    }
}

void Ui::modal_window_ok_size_constraints(
    const char* title,
    std::function<void()>&& contents,
    glm::vec2 min_size,
    glm::vec2 max_size
) {
    ImGui::OpenPopup(title);

    ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetWorkCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
    ImGui::SetNextWindowSizeConstraints(ImVec2(min_size.x, min_size.y), ImVec2(max_size.x, max_size.y));

    if (ImGui::BeginPopupModal(title, nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_AlwaysAutoResize)) {
        contents();

        ImGui::Dummy(ImVec2(rem(8.0f), rem(0.5f)));

        const float ok_button_width {rem(6.0f)};
        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - ok_button_width) * 0.5f);

        if (ImGui::Button("Ok", ImVec2(ok_button_width, 0.0f))) {
            ImGui::CloseCurrentPopup();
            m_modal_window_queue.pop_front();
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
    ctx.add_task_immediate([&ctx, scale]() {
        set_scale(ctx, scale);

        return sm::Task::Result::Done;
    });
}

void Ui::set_anti_aliasing_task(sm::Ctx& ctx, int samples) {
    ctx.add_task_immediate([&ctx, samples]() {
        ctx.set_renderer_samples(samples);

        return sm::Task::Result::Done;
    });
}

void Ui::set_shadow_quality_task(sm::Ctx& ctx, int size) {
    ctx.add_task_immediate([&ctx, size]() {
        ctx.set_renderer_shadow_map_size(size);

        return sm::Task::Result::Done;
    });
}

void Ui::set_anisotropic_filtering_task(sm::Ctx& ctx, GameScene& game_scene) {
    ctx.add_task_immediate([&game_scene]() {
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

    const auto file_path {ctx.path_assets("fonts/OpenSans/OpenSans-Semibold.ttf")};

    const auto font {io.Fonts->AddFontFromFileTTF(
        file_path.string().c_str(),
        font_size,
        nullptr,
        ranges.Data
    )};

    if (font == nullptr) {
        SM_THROW_ERROR(sm::ApplicationError, "Could not load font: `{}`", file_path.string());
    }

    io.FontDefault = font;
    io.Fonts->Build();

    // This was also not very documented :P
    ctx.invalidate_dear_imgui_texture();
}

void Ui::set_style() {
    ImGuiStyle& style {ImGui::GetStyle()};

    style = ImGuiStyle();
    ImGui::StyleColorsClassic();

    style.Colors[ImGuiCol_Text]                   = ImVec4(0.82f, 0.82f, 0.82f, 1.00f);
    style.Colors[ImGuiCol_FrameBgHovered]         = ImVec4(0.69f, 0.52f, 0.47f, 0.40f);
    style.Colors[ImGuiCol_FrameBgActive]          = ImVec4(0.64f, 0.46f, 0.41f, 0.69f);
    style.Colors[ImGuiCol_TitleBg]                = ImVec4(0.54f, 0.33f, 0.27f, 0.83f);
    style.Colors[ImGuiCol_TitleBgActive]          = ImVec4(0.63f, 0.39f, 0.32f, 0.87f);
    style.Colors[ImGuiCol_TitleBgCollapsed]       = ImVec4(0.80f, 0.49f, 0.40f, 0.20f);
    style.Colors[ImGuiCol_MenuBarBg]              = ImVec4(0.55f, 0.43f, 0.40f, 0.80f);
    style.Colors[ImGuiCol_ScrollbarBg]            = ImVec4(0.30f, 0.20f, 0.20f, 0.60f);
    style.Colors[ImGuiCol_ScrollbarGrab]          = ImVec4(0.80f, 0.49f, 0.40f, 0.30f);
    style.Colors[ImGuiCol_ScrollbarGrabHovered]   = ImVec4(0.80f, 0.49f, 0.40f, 0.40f);
    style.Colors[ImGuiCol_ScrollbarGrabActive]    = ImVec4(0.80f, 0.49f, 0.39f, 0.60f);
    style.Colors[ImGuiCol_SliderGrabActive]       = ImVec4(0.80f, 0.49f, 0.39f, 0.60f);
    style.Colors[ImGuiCol_Button]                 = ImVec4(0.61f, 0.35f, 0.35f, 0.62f);
    style.Colors[ImGuiCol_ButtonHovered]          = ImVec4(0.71f, 0.40f, 0.40f, 0.79f);
    style.Colors[ImGuiCol_ButtonActive]           = ImVec4(0.80f, 0.46f, 0.46f, 1.00f);
    style.Colors[ImGuiCol_Header]                 = ImVec4(0.90f, 0.52f, 0.40f, 0.45f);
    style.Colors[ImGuiCol_HeaderHovered]          = ImVec4(0.90f, 0.56f, 0.45f, 0.80f);
    style.Colors[ImGuiCol_HeaderActive]           = ImVec4(0.87f, 0.68f, 0.53f, 0.80f);
    style.Colors[ImGuiCol_SeparatorHovered]       = ImVec4(0.70f, 0.63f, 0.60f, 1.00f);
    style.Colors[ImGuiCol_SeparatorActive]        = ImVec4(0.90f, 0.75f, 0.70f, 1.00f);
    style.Colors[ImGuiCol_ResizeGripHovered]      = ImVec4(1.00f, 0.78f, 0.78f, 0.60f);
    style.Colors[ImGuiCol_ResizeGripActive]       = ImVec4(1.00f, 0.78f, 0.78f, 0.90f);
    style.Colors[ImGuiCol_TabHovered]             = ImVec4(0.90f, 0.56f, 0.45f, 0.80f);
    style.Colors[ImGuiCol_Tab]                    = ImVec4(0.68f, 0.42f, 0.33f, 0.78f);
    style.Colors[ImGuiCol_TabSelected]            = ImVec4(0.73f, 0.48f, 0.40f, 0.84f);
    style.Colors[ImGuiCol_TabSelectedOverline]    = ImVec4(0.87f, 0.61f, 0.53f, 0.80f);
    style.Colors[ImGuiCol_TabDimmed]              = ImVec4(0.57f, 0.35f, 0.28f, 0.82f);
    style.Colors[ImGuiCol_TabDimmedSelected]      = ImVec4(0.65f, 0.42f, 0.35f, 0.84f);
    style.Colors[ImGuiCol_TabDimmedSelectedOverline]  = ImVec4(0.87f, 0.61f, 0.53f, 0.00f);
    style.Colors[ImGuiCol_TableHeaderBg]          = ImVec4(0.38f, 0.30f, 0.27f, 1.00f);
    style.Colors[ImGuiCol_TableBorderStrong]      = ImVec4(0.45f, 0.34f, 0.31f, 1.00f);
    style.Colors[ImGuiCol_TextLink]               = ImVec4(0.87f, 0.61f, 0.53f, 0.80f);
    style.Colors[ImGuiCol_TextSelectedBg]         = ImVec4(1.00f, 0.24f, 0.00f, 0.35f);
    style.Colors[ImGuiCol_NavCursor]              = ImVec4(0.90f, 0.56f, 0.45f, 0.80f);

    style.FramePadding = ImVec2(8.0f, 4.0f);

    style.WindowPadding = ImVec2(12.0f, 12.0f);
    style.WindowRounding = 8.0f;
    style.ChildRounding = 8.0f;
    style.FrameRounding = 8.0f;
    style.PopupRounding = 8.0f;
    style.ScrollbarRounding = 8.0f;
    style.GrabRounding = 8.0f;
    style.TabRounding = 8.0f;

    style.WindowTitleAlign = ImVec2(0.5f, 0.5f);

    style.DisplayWindowPadding = ImVec2(30.0f, 30.0f);
    style.DisplaySafeAreaPadding = ImVec2(4.0f, 4.0f);
}

bool Ui::resign_available(GameScene& game_scene) {
    return (
        game_scene.get_game_session() &&
        game_scene.get_game_session()->get_remote_joined() &&
        game_scene.get_game_state() != GameState::Ready &&
        game_scene.get_game_state() != GameState::Over
    );
}

PlayerColor Ui::resign_player(GameScene& game_scene) {
    assert(game_scene.get_game_session());

    return opponent(static_cast<PlayerColor>(game_scene.get_game_options().remote_color));
}

bool Ui::offer_draw_available(GameScene& game_scene) {
    return (
        game_scene.get_game_session() &&
        game_scene.get_game_session()->get_remote_joined() &&
        game_scene.get_board().get_player_color() == static_cast<PlayerColor>(game_scene.get_game_options().remote_color) &&
        game_scene.get_game_state() != GameState::Ready &&
        game_scene.get_game_state() != GameState::Over
    );
}

bool Ui::accept_draw_available(GameScene& game_scene) {
    return (
        game_scene.get_game_session() &&
        game_scene.get_game_session()->get_remote_joined() &&
        game_scene.get_game_session()->get_remote_offered_draw() &&
        game_scene.get_game_state() != GameState::Ready &&
        game_scene.get_game_state() != GameState::Over
    );
}

bool Ui::join_game_available(GameScene& game_scene) {
    return (
        !game_scene.get_game_session() &&
        !std::any_of(std::cbegin(m_session_id), std::prev(std::cend(m_session_id)), [](char c) { return c == 0; })
    );
}
