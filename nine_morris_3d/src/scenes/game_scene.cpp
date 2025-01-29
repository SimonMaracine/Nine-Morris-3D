#include "scenes/game_scene.hpp"

#include <limits>

#include <nine_morris_3d_engine/external/resmanager.h++>
#include <protocol.hpp>

#include "global.hpp"
#include "default_address.hpp"

void GameScene::on_start() {
    ctx.connect_event<sm::WindowResizedEvent, &GameScene::on_window_resized>(this);
    ctx.connect_event<sm::KeyReleasedEvent, &GameScene::on_key_released>(this);
    ctx.connect_event<sm::MouseButtonPressedEvent, &GameScene::on_mouse_button_pressed>(this);
    ctx.connect_event<sm::MouseButtonReleasedEvent, &GameScene::on_mouse_button_released>(this);

    ctx.set_renderer_clear_color(glm::vec3(0.1f, 0.1f, 0.1f));

    m_ui.initialize(ctx);

    setup_camera();
    setup_skybox();
    setup_lights();

    scene_setup();
    start_engine();

    const auto& g {ctx.global<Global>()};

    if (g.options.default_address_port) {
        connect(DEFAULT_ADDRESS, DEFAULT_PORT);
    } else {
        connect(g.options.address, g.options.port);
    }

    ctx.add_task_delayed([this]() {
        client_ping();

        return sm::Task::Result::Repeat;
    }, 3.0);
}

void GameScene::on_stop() {
    stop_engine();

    m_camera_controller.disconnect_events(ctx);
    ctx.disconnect_events(this);
}

void GameScene::on_update() {
    m_camera_controller.update_controls(ctx.get_delta(), ctx);
    m_camera_controller.update_camera(ctx.get_delta());

    ctx.capture(m_camera, m_camera_controller.get_position());
    ctx.capture(m_camera_2d);

    ctx.add_light(m_directional_light);
    ctx.environment(m_skybox);

    if (m_ui.get_show_information()) {
        ctx.show_information_text();
    }

    // Origin
    ctx.debug_add_line(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ctx.debug_add_line(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ctx.debug_add_line(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Light
    ctx.debug_add_lamp(glm::normalize(-m_directional_light.direction) * 15.0f, glm::vec3(0.6f));

    m_clock.update();

    if (m_game_state != GameState::Ready && m_game_state != GameState::Over) {
        if (m_clock.get_white_time() == 0) {
            timeout(PlayerColorWhite);
        }

        if (m_clock.get_black_time() == 0) {
            timeout(PlayerColorBlack);
        }
    }

    scene_update();
    update_game_state();

    // This needs to be called after all scene is updated
    ctx.shadow(m_shadow_box);

    update_connection_state();
}

void GameScene::on_fixed_update() {
    m_camera_controller.update_friction();

    scene_fixed_update();
}

void GameScene::on_imgui_update() {
    m_ui.update(ctx, *this);

    scene_imgui_update();
}

void GameScene::reload_and_set_skybox() {
    ctx.add_task_async([this](sm::AsyncTask& task) {
        try {
            reload_skybox_texture_data();
        } catch (const sm::RuntimeError&) {
            task.set_done(std::current_exception());
            return;
        }

        ctx.add_task_immediate([this]() {
            setup_skybox(true);
            setup_lights();
            m_ui.set_loading_skybox_done();

            return sm::Task::Result::Done;
        });

        task.set_done();
    });
}

void GameScene::reload_and_set_textures() {
    ctx.add_task_async([this](sm::AsyncTask& task) {
        try {
            reload_skybox_texture_data();
            reload_scene_texture_data();
        } catch (const sm::RuntimeError&) {
            task.set_done(std::current_exception());
            return;
        }

        ctx.add_task_immediate([this]() {
            setup_skybox(true);
            reload_and_set_scene_textures();

            return sm::Task::Result::Done;
        });

        task.set_done();
    });
}

void GameScene::connect(const std::string& address, std::uint16_t port, bool reconnect) {
    auto& g {ctx.global<Global>()};

    // This prevents unwanted disconnections and reconnections
    if (!reconnect && g.connection_state == ConnectionState::Connected) {
        return;
    }

    // To prevent bad states and desynchronizations
    reset_session_and_game();

    g.client.disconnect();

    try {
        LOG_DIST_INFO("Connecting to {}:{}...", address, port);
        g.client.connect(address, port);
        g.connection_state = ConnectionState::Connecting;
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
    }
}

void GameScene::connect(const std::string& address, const std::string& port, bool reconnect) {
    try {
        connect(address, sm::utils::string_to_unsigned_short(port), reconnect);
    } catch (const sm::OtherError& e) {
        LOG_DIST_ERROR("Invalid port: {}", e.what());
    }
}

void GameScene::client_request_game_session() {
    auto& g {ctx.global<Global>()};

    protocol::Client_RequestGameSession payload;
    payload.player_name = g.options.name;
    payload.remote_player_type = protocol::Player(m_game_options.online.remote_color);

    networking::Message message {protocol::message::Client_RequestGameSession};

    try {
        message.write(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    try {
        g.client.send_message(message);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
        return;
    }

    LOG_DEBUG("Requested a new game session");
    m_ui.push_popup_window(PopupWindow::WaitServerAcceptGameSession);
}

void GameScene::client_quit_game_session() {
    assert(m_game_session);

    protocol::Client_QuitGameSession payload;
    payload.session_id = m_game_session->get_session_id();

    networking::Message message {protocol::message::Client_QuitGameSession};

    try {
        message.write(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    auto& g {ctx.global<Global>()};

    try {
        g.client.send_message(message);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
        return;
    }

    m_game_session.reset();
}

void GameScene::client_request_join_game_session(const std::string& session_id) {
    auto& g {ctx.global<Global>()};

    protocol::Client_RequestJoinGameSession payload;

    try {
        payload.session_id = sm::utils::string_to_unsigned_short(session_id);
        payload.player_name = g.options.name;
    } catch (const sm::OtherError& e) {
        LOG_DIST_ERROR("Invalid code: {}", e.what());
        m_ui.push_popup_window(PopupWindow::JoinGameSessionError, "Invalid code");
        return;
    }

    networking::Message message {protocol::message::Client_RequestJoinGameSession};

    try {
        message.write(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    try {
        g.client.send_message(message);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
        return;
    }

    LOG_DEBUG("Requested to join a game session");
    m_ui.push_popup_window(PopupWindow::WaitServerAcceptJoinGameSession);
}

void GameScene::client_play_move(const std::string& move) {
    assert(m_game_session);

    protocol::Client_PlayMove payload;
    payload.session_id = m_game_session->get_session_id();
    payload.move = move;

    networking::Message message {protocol::message::Client_PlayMove};

    try {
        message.write(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    auto& g {ctx.global<Global>()};

    try {
        g.client.send_message(message);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
    }
}

void GameScene::client_resign() {
    assert(m_game_session);

    protocol::Client_Resign payload;
    payload.session_id = m_game_session->get_session_id();

    networking::Message message {protocol::message::Client_Resign};

    try {
        message.write(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    auto& g {ctx.global<Global>()};

    try {
        g.client.send_message(message);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
    }
}

void GameScene::client_rematch() {
    assert(m_game_session);

    protocol::Client_Rematch payload;
    payload.session_id = m_game_session->get_session_id();

    networking::Message message {protocol::message::Client_Rematch};

    try {
        message.write(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    auto& g {ctx.global<Global>()};

    try {
        g.client.send_message(message);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
        return;
    }

    m_ui.push_popup_window(PopupWindow::WaitRemoteRematch);
}

void GameScene::client_cancel_rematch() {
    assert(m_game_session);

    protocol::Client_CancelRematch payload;
    payload.session_id = m_game_session->get_session_id();

    networking::Message message {protocol::message::Client_CancelRematch};

    try {
        message.write(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    auto& g {ctx.global<Global>()};

    try {
        g.client.send_message(message);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
    }
}

void GameScene::client_send_message(const std::string& message_) {
    assert(m_game_session);

    protocol::Client_SendMessage payload;
    payload.session_id = m_game_session->get_session_id();
    payload.message = message_;

    networking::Message message {protocol::message::Client_SendMessage};

    try {
        message.write(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    auto& g {ctx.global<Global>()};

    try {
        g.client.send_message(message);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
    }
}

void GameScene::on_window_resized(const sm::WindowResizedEvent& event) {
    m_camera.set_projection(event.width, event.height, LENS_FOV, LENS_NEAR, LENS_FAR);
    m_camera_2d.set_projection(0, event.width, 0, event.height);
}

void GameScene::on_key_released(const sm::KeyReleasedEvent& event) {
    if (event.key == sm::Key::Space) {
        m_camera_controller.go_towards_position(m_default_camera_position);
    }
}

void GameScene::on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        if (m_game_state == GameState::HumanThinking) {
            get_board().user_click_press();
        }
    }
}

void GameScene::on_mouse_button_released(const sm::MouseButtonReleasedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        if (m_game_state == GameState::HumanThinking) {
            get_board().user_click_release();
        }
    }
}

void GameScene::stop_engine() {
    if (!m_engine) {
        return;
    }

    try {
        m_engine->uninitialize();
    } catch (const EngineError& e) {
        LOG_DIST_ERROR("Engine error: {}", e.what());
    }

    m_engine.reset();
}

void GameScene::assert_engine_game_over() {
    assert(m_engine);

    try {
        m_engine->start_thinking(
            get_setup_position(),
            m_move_list.get_moves(),
            std::nullopt,
            std::nullopt,
            100
        );

        while (true) {
            const auto best_move {m_engine->done_thinking()};

            if (!best_move) {
                continue;
            }

            if (!m_engine->is_null_move(*best_move)) {
                SM_THROW_ERROR(sm::ApplicationError, "The GUI calls game over, but the engine doesn't agree");
            }

            break;
        }
    } catch (const EngineError& e) {
        engine_error(e);
    }
}

void GameScene::engine_error(const EngineError& e) {
    LOG_DIST_ERROR("Engine error: {}", e.what());
    m_engine.reset();
    m_ui.push_popup_window(PopupWindow::EngineError);
}

void GameScene::update_game_state() {
    switch (m_game_state) {
        case GameState::Ready:
            break;
        case GameState::Start:
            m_clock.start();
            m_game_state = GameState::NextTurn;

            break;
        case GameState::NextTurn: {
            switch (get_player_type()) {
                case GamePlayer::Human:
                    m_game_state = GameState::HumanThinking;
                    break;
                case GamePlayer::Computer:
                    m_game_state = GameState::ComputerStartThinking;
                    break;
                case GamePlayer::Remote:
                    m_game_state = GameState::RemoteThinking;
                    break;
            }

            break;
        }
        case GameState::HumanThinking:
            break;
        case GameState::ComputerStartThinking:
            assert(m_engine);

            try {
                m_engine->start_thinking(
                    get_setup_position(),
                    m_move_list.get_moves(),
                    m_clock.get_white_time(),
                    m_clock.get_black_time(),
                    std::nullopt
                );
            } catch (const EngineError& e) {
                engine_error(e);
                m_game_state = GameState::Stop;
                break;
            }

            m_game_state = GameState::ComputerThinking;

            break;
        case GameState::ComputerThinking: {
            assert(m_engine);

            std::optional<std::string> best_move;

            try {
                best_move = m_engine->done_thinking();
            } catch (const EngineError& e) {
                engine_error(e);
                m_game_state = GameState::Stop;
                break;
            }

            if (best_move) {
                if (m_engine->is_null_move(*best_move)) {
                    if (get_board().get_game_over() == GameOver::None) {
                        SM_THROW_ERROR(sm::ApplicationError, "The engine calls game over, but the GUI doesn't agree");
                    }
                } else {
                    play_move(*best_move);
                }
            }

            break;
        }
        case GameState::RemoteThinking:
            break;
        case GameState::FinishTurn:
            if (get_board().is_turn_finished()) {
                m_clock.switch_turn();

                if (get_board().get_game_over() != GameOver::None) {
                    assert_engine_game_over();
                    m_ui.push_popup_window(PopupWindow::GameOver);
                    m_game_state = GameState::Stop;
                } else {
                    m_game_state = GameState::NextTurn;
                }
            }

            break;
        case GameState::Stop:
            m_clock.stop();
            m_game_state = GameState::Over;

            break;
        case GameState::Over:
            break;
    }
}

void GameScene::setup_camera() {
    const auto& g {ctx.global<Global>()};

    m_camera_controller = PointCameraController(
        m_camera,
        ctx.get_window_width(),
        ctx.get_window_height(),
        glm::vec3(0.0f),
        8.0f,
        47.0f,
        g.options.camera_sensitivity
    );

    m_camera_controller.connect_events(ctx);

    m_default_camera_position = m_camera_controller.get_position();
    m_camera_controller.set_distance_to_point(m_camera_controller.get_distance_to_point() + 1.0f);
    m_camera_controller.go_towards_position(m_default_camera_position);

    m_camera_2d.set_projection(0, ctx.get_window_width(), 0, ctx.get_window_height());
}

void GameScene::setup_skybox(bool reload) {
    m_skybox.texture = load_skybox_texture_cubemap(reload);
}

void GameScene::setup_lights() {
    const auto& g {ctx.global<Global>()};

    switch (g.options.skybox) {
        case SkyboxNone:
            m_directional_light.direction = glm::normalize(glm::vec3(0.307f, -0.901f, 0.307f));
            m_directional_light.ambient_color = glm::vec3(0.1f);
            m_directional_light.diffuse_color = glm::vec3(0.5f);
            m_directional_light.specular_color = glm::vec3(0.8f);
            break;
        case SkyboxField:
            m_directional_light.direction = glm::normalize(glm::vec3(-0.525f, -0.405f, -0.748f));
            m_directional_light.ambient_color = glm::vec3(0.25f);
            m_directional_light.diffuse_color = glm::vec3(0.7f);
            m_directional_light.specular_color = glm::vec3(1.0f);
            break;
        case SkyboxAutumn:
            m_directional_light.direction = glm::normalize(glm::vec3(0.370f, -0.925f, -0.092f));
            m_directional_light.ambient_color = glm::vec3(0.2f);
            m_directional_light.diffuse_color = glm::vec3(0.7f);
            m_directional_light.specular_color = glm::vec3(0.9f);
            break;
    }
}

void GameScene::reload_skybox_texture_data() const {
    // Global options must have been set to the desired skybox

    const auto& g {ctx.global<Global>()};

    sm::TexturePostProcessing post_processing;
    post_processing.flip = false;

    if (g.options.texture_quality == TextureQualityHalf) {
        post_processing.size = sm::TextureSize::Half;
    }

    switch (g.options.skybox) {
        case SkyboxNone:
            break;
        case SkyboxField:
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/px.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/nx.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/py.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/ny.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/pz.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/field/nz.png"), post_processing);
            break;
        case SkyboxAutumn:
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/px.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/nx.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/py.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/ny.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/pz.png"), post_processing);
            ctx.reload_texture_data(ctx.path_assets("textures/skybox/autumn/nz.png"), post_processing);
            break;
    }
}

std::shared_ptr<sm::GlTextureCubemap> GameScene::load_skybox_texture_cubemap(bool reload) const {
    const auto& g {ctx.global<Global>()};

    const auto load_or_reload {
        [this, reload](sm::Id id) {
            if (reload) {
                return ctx.reload_texture_cubemap(
                    id,
                    {
                        ctx.get_texture_data("px.png"_H),
                        ctx.get_texture_data("nx.png"_H),
                        ctx.get_texture_data("py.png"_H),
                        ctx.get_texture_data("ny.png"_H),
                        ctx.get_texture_data("pz.png"_H),
                        ctx.get_texture_data("nz.png"_H)
                    },
                    sm::TextureFormat::Srgb8Alpha8
                );
            } else {
                return ctx.load_texture_cubemap(
                    id,
                    {
                        ctx.get_texture_data("px.png"_H),
                        ctx.get_texture_data("nx.png"_H),
                        ctx.get_texture_data("py.png"_H),
                        ctx.get_texture_data("ny.png"_H),
                        ctx.get_texture_data("pz.png"_H),
                        ctx.get_texture_data("nz.png"_H)
                    },
                    sm::TextureFormat::Srgb8Alpha8
                );
            }
        }
    };

    switch (g.options.skybox) {
        case SkyboxNone:
            return nullptr;
        case SkyboxField:
            return load_or_reload("field"_H);
        case SkyboxAutumn:
            return load_or_reload("autumn"_H);
    }

    return {};
}

void GameScene::connection_error(const networking::ConnectionError& e) {
    auto& g {ctx.global<Global>()};

    LOG_DIST_ERROR("Connection error: {}", e.what());

    // To prevent bad states and desynchronizations
    reset_session_and_game();

    g.connection_state = ConnectionState::Disconnected;

    m_ui.clear_popup_window();  // The user may already be blocked in a popup window
    m_ui.push_popup_window(PopupWindow::ConnectionError);
}

void GameScene::serialization_error(const networking::SerializationError& e) {
    auto& g {ctx.global<Global>()};

    LOG_DIST_CRITICAL("Serialization error: {}", e.what());

    // To prevent bad states and desynchronizations
    reset_session_and_game();

    g.connection_state = ConnectionState::Disconnected;

    g.client.disconnect();
    LOG_DIST_INFO("Disconnected from server");

    m_ui.clear_popup_window();  // The user may already be blocked in a popup window
    m_ui.push_popup_window(PopupWindow::ConnectionError);
}

void GameScene::reset_session_and_game() {
    if (m_game_session) {
        m_game_session.reset();
        reset();
    }
}

void GameScene::update_connection_state() {
    auto& g {ctx.global<Global>()};

    try {
        switch (g.connection_state) {
            case ConnectionState::Disconnected:
                break;
            case ConnectionState::Connecting:
                if (g.client.connection_established()) {
                    LOG_DIST_INFO("Connected to server");
                    g.connection_state = ConnectionState::Connected;
                }

                break;
            case ConnectionState::Connected:
                while (g.client.available_messages()) {
                    handle_message(g.client.next_message());
                }

                break;
        }
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
    }
}

void GameScene::handle_message(const networking::Message& message) {
    switch (message.id()) {
        case protocol::message::Server_Ping:
            server_ping(message);
            break;
        case protocol::message::Server_AcceptGameSession:
            server_accept_game_session(message);
            break;
        case protocol::message::Server_DenyGameSession:
            server_deny_game_session(message);
            break;
        case protocol::message::Server_AcceptJoinGameSession:
            server_accept_join_game_session(message);
            break;
        case protocol::message::Server_DenyJoinGameSession:
            server_deny_join_game_session(message);
            break;
        case protocol::message::Server_RemoteJoinedGameSession:
            server_remote_joined_game_session(message);
            break;
        case protocol::message::Server_RemoteQuitGameSession:
            server_remote_quit_game_session(message);
            break;
        case protocol::message::Server_RemotePlayedMove:
            server_remote_played_move(message);
            break;
        case protocol::message::Server_RemoteResigned:
            server_remote_resigned(message);
            break;
        case protocol::message::Server_Rematch:
            server_rematch(message);
            break;
        case protocol::message::Server_CancelRematch:
            server_cancel_rematch(message);
            break;
        case protocol::message::Server_RemoteSentMessage:
            server_remote_sent_message(message);
            break;
    }
}

void GameScene::client_ping() {
    auto& g {ctx.global<Global>()};

    protocol::Client_Ping payload;
    payload.time = std::chrono::system_clock::now();

    networking::Message message {protocol::message::Client_Ping};

    try {
        message.write(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    try {
        g.client.send_message(message);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
    }
}

void GameScene::server_ping(const networking::Message& message) {
    protocol::Server_Ping payload;

    try {
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    LOG_DEBUG("Ping: {}", std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - payload.time).count());
}

void GameScene::server_accept_game_session(const networking::Message& message) {
    protocol::Server_AcceptGameSession payload;

    try {
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    m_game_session = GameSession(payload.session_id);

    m_ui.clear_popup_window();
    m_ui.push_popup_window(PopupWindow::WaitRemoteJoinGameSession);
}

void GameScene::server_deny_game_session(const networking::Message& message) {
    protocol::Server_DenyGameSession payload;

    try{
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    m_ui.clear_popup_window();
    m_ui.push_popup_window(PopupWindow::NewGameSessionError, protocol::error_code_string(payload.error_code));
}

void GameScene::server_accept_join_game_session(const networking::Message& message) {
    protocol::Server_AcceptJoinGameSession payload;

    try {
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    reset(payload.moves);

    m_game_session = GameSession(payload.session_id);
    m_game_session->remote_join(payload.remote_player_name);
    m_game_session->set_messages(payload.messages);

    m_game_options.online.remote_color = PlayerColor(payload.remote_player_type);

    // Unblock from waiting
    m_ui.clear_popup_window();

    m_game_state = GameState::Start;
}

void GameScene::server_deny_join_game_session(const networking::Message& message) {
    protocol::Server_DenyJoinGameSession payload;

    try {
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    m_ui.clear_popup_window();
    m_ui.push_popup_window(PopupWindow::JoinGameSessionError, protocol::error_code_string(payload.error_code));
}

void GameScene::server_remote_joined_game_session(const networking::Message& message) {
    // The user may have quit the session in the meantime
    if (!m_game_session) {
        return;
    }

    protocol::Server_RemoteJoinedGameSession payload;

    try {
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    m_game_session->remote_join(payload.remote_player_name);

    // Prevent from clearing unwanted popup windows
    if (m_ui.get_popup_window() == PopupWindow::WaitRemoteJoinGameSession) {
        // Unblock from waiting
        m_ui.clear_popup_window();
    }

    m_game_state = GameState::Start;
}

void GameScene::server_remote_quit_game_session(const networking::Message&) {
    // The user may have quit the session in the meantime
    if (!m_game_session) {
        return;
    }

    m_game_session->remote_quit();
}

void GameScene::server_remote_played_move(const networking::Message& message) {
    // The user may have quit the session in the meantime
    if (!m_game_session) {
        return;
    }

    protocol::Server_RemotePlayedMove payload;

    try {
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    assert(m_game_state == GameState::RemoteThinking);

    play_move(payload.move);
}

void GameScene::server_remote_resigned(const networking::Message&) {
    // The user may have quit the session in the meantime
    if (!m_game_session) {
        return;
    }

    resign(PlayerColor(m_game_options.online.remote_color));
}

void GameScene::server_remote_sent_message(const networking::Message& message) {
    // The user may have quit the session in the meantime
    if (!m_game_session) {
        return;
    }

    protocol::Server_RemoteSentMessage payload;

    try {
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    m_game_session->remote_sent_message(payload.message);
}

void GameScene::server_rematch(const networking::Message& message) {
    // The user may have quit the session in the meantime
    if (!m_game_session) {
        return;
    }

    protocol::Server_Rematch payload;

    try {
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    m_game_options.online.remote_color = PlayerColor(payload.remote_player_type);

    m_game_state = GameState::Start;

    assert(m_ui.get_popup_window() == PopupWindow::WaitRemoteRematch);

    // Unblock from waiting
    m_ui.clear_popup_window();
}

void GameScene::server_cancel_rematch(const networking::Message&) {
    // The user may have quit the session in the meantime
    if (!m_game_session) {
        return;
    }

    assert(m_ui.get_popup_window() == PopupWindow::WaitRemoteRematch);

    // Unblock from waiting
    m_ui.clear_popup_window();
}
