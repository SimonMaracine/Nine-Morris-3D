#include "scenes/game_scene.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>
#include <protocol.hpp>

#include "global.hpp"
#include "ver.hpp"

void GameScene::on_start() {
    ctx.connect_event<sm::KeyReleasedEvent, &GameScene::on_key_released>(this);
    ctx.connect_event<sm::MouseButtonPressedEvent, &GameScene::on_mouse_button_pressed>(this);
    ctx.connect_event<sm::MouseButtonReleasedEvent, &GameScene::on_mouse_button_released>(this);

    ctx.set_renderer_clear_color(glm::vec3(0.1f, 0.1f, 0.1f));

    m_ui.initialize(ctx);

    setup_camera();
    setup_skybox();
    setup_lights();

    scene_setup();
    load_icons();
    load_sounds();
    start_engine();

    auto& g {ctx.global<Global>()};

    if (!g.client.get_attempted_connection()) {
        if (g.options.default_address_port) {
            connect();
        } else {
            connect(g.options.address, g.options.port);
        }
    }

    ctx.add_task_delayed([this]() {
        const auto& g {ctx.global<Global>()};

        if (g.client.get_connection_state() == ConnectionState::Connected) {
            client_ping();
        }

        return sm::Task::Result::Repeat;
    }, 5.0);

    ctx.add_task_delayed([this]() {
        const auto& g {ctx.global<Global>()};

        if (g.client.get_connection_state() != ConnectionState::Connected) {
            return sm::Task::Result::Repeat;
        }

        if (!m_game_session) {
            return sm::Task::Result::Repeat;
        }

        if (m_game_state == GameState::Ready || m_game_state == GameState::Over) {
            return sm::Task::Result::Repeat;
        }

        if (get_board().get_player_color() == static_cast<PlayerColor>(m_game_options.remote_color)) {
            return sm::Task::Result::Repeat;
        }

        client_update_turn_time(get_board().get_player_color() == PlayerColorWhite ? m_clock.get_white_time() : m_clock.get_black_time());

        return sm::Task::Result::Repeat;
    }, 3.0);

    sm::Ctx::play_audio_sound(m_sound_new_game);
}

void GameScene::on_stop() {
    if (m_game_session) {
        if (m_game_session->get_remote_joined() && m_game_state != GameState::Ready && m_game_state != GameState::Over) {
            client_resign();
        }

        client_leave_game_session();
    }

    stop_engine();

    m_camera_controller->disconnect_events(ctx);
    ctx.disconnect_events(this);
}

void GameScene::on_update() {
    if (m_ui.get_show_information()) {
        ctx.show_information_text();
    }

    // Origin
    ctx.render_3d()->debug_add_line(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    ctx.render_3d()->debug_add_line(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    ctx.render_3d()->debug_add_line(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    // Light
    ctx.render_3d()->debug_add_lamp(glm::normalize(-ctx.render_3d()->directional_light.direction) * 15.0f, glm::vec3(0.6f));

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
    update_connection_state();
}

void GameScene::on_fixed_update() {
    m_camera_controller->update_friction();

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

void GameScene::reset(const std::string& string, const std::vector<std::string>& moves) {
    if (m_engine) {
        try {
            m_engine->stop_thinking();  // Stop the engine first
            m_engine->new_game();
            m_engine->synchronize();
        } catch (const EngineError& e) {
            engine_error(e);
            // Reset the other stuff anyway
        }
    }

    reset_board(string);

    m_game_state = GameState::Ready;
    m_clock.reset(clock_time(m_game_options.time_enum));
    m_move_list.clear();

    if (second_player_starting()) {
        m_clock.switch_turn();
        m_move_list.skip_first(true);
    }

    // Play the moves offscreen
    get_board().enable_move_callback(false);
    get_board().enable_move_animations(false);

    for (const auto& move : moves) {
        play_move(move);
        m_clock.switch_turn();
        m_move_list.push(move);
    }

    get_board().enable_move_animations(true);
    get_board().enable_move_callback(true);

    // After the played moves, the game might be already over
    if (get_board().get_game_over() != GameOver::None) {
        m_game_state = GameState::Over;
    }

    // Place the pieces into their places
    get_board().setup_pieces();

    // The correct colors should be set before calling reset()
    reset_camera_position();

    sm::Ctx::play_audio_sound(m_sound_new_game);
}

void GameScene::reset_camera_position() {
    const auto& g {ctx.global<Global>()};

    switch (g.options.game_type) {
        case GameTypeLocalHumanVsHuman:
            m_camera_controller->go_towards_position(m_white_camera_position);
            break;
        case GameTypeLocalHumanVsComputer:
            switch (static_cast<PlayerColor>(m_game_options.computer_color)) {
                case PlayerColorWhite:
                    m_camera_controller->go_towards_position(m_black_camera_position);
                    break;
                case PlayerColorBlack:
                    m_camera_controller->go_towards_position(m_white_camera_position);
                    break;
            }
            break;
        case GameTypeOnline:
            switch (static_cast<PlayerColor>(m_game_options.remote_color)) {
                case PlayerColorWhite:
                    m_camera_controller->go_towards_position(m_black_camera_position);
                    break;
                case PlayerColorBlack:
                    m_camera_controller->go_towards_position(m_white_camera_position);
                    break;
            }
            break;
    }
}

void GameScene::connect(const std::string& address, std::uint16_t port) {
    auto& g {ctx.global<Global>()};

    try {
        g.client.connect(address, port);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
    }
}

void GameScene::connect(const std::string& address, const std::string& port) {
    try {
        connect(address, sm::utils::string_to_unsigned_short(port));
    } catch (const sm::RuntimeError& e) {
        LOG_DIST_ERROR("Invalid port: {}", e.what());
    }
}

void GameScene::connect() {
    auto& g {ctx.global<Global>()};

    // To prevent bad states and desynchronizations
    reset_session_and_game();

    try {
        g.client.connect();
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
    }
}

void GameScene::disconnect() {
    auto& g {ctx.global<Global>()};

    // To prevent bad states and desynchronizations
    reset_session_and_game();

    g.client.disconnect();
}

void GameScene::client_hello() {
    protocol::Client_Hello payload;
    payload.version = version_number();

    networking::Message message {protocol::message::Client_Hello};

    if (!try_write_message(message, payload)) {
        return;
    }

    try_send_message(message);
}

void GameScene::client_ping() {
    protocol::Client_Ping payload;
    payload.time = std::chrono::system_clock::now();

    networking::Message message {protocol::message::Client_Ping};

    if (!try_write_message(message, payload)) {
        return;
    }

    try_send_message(message);
}

void GameScene::client_request_game_session() {
    auto& g {ctx.global<Global>()};

    protocol::Client_RequestGameSession payload;
    payload.player_name = g.options.name;
    payload.remote_player = protocol::Player(m_game_options.remote_color);
    payload.initial_time = m_clock.get_white_time();  // Players have equal time
    payload.game_mode = protocol::GameMode(g.options.game_mode);

    networking::Message message {protocol::message::Client_RequestGameSession};

    if (!try_write_message(message, payload)) {
        return;
    }

    if (!try_send_message(message)) {
        return;
    }

    LOG_DEBUG("Requested a new game session");
    m_ui.push_modal_window(ModalWindowWaitServerAcceptGameSession);
}

void GameScene::client_leave_game_session() {
    assert(m_game_session);

    protocol::Client_LeaveGameSession payload;
    payload.session_id = m_game_session->get_session_id();

    networking::Message message {protocol::message::Client_LeaveGameSession};

    if (!try_write_message(message, payload)) {
        return;
    }

    if (!try_send_message(message)) {
        return;
    }

    m_game_session.reset();
}

void GameScene::client_request_join_game_session(const std::string& session_id) {
    auto& g {ctx.global<Global>()};

    protocol::Client_RequestJoinGameSession payload;

    try {
        payload.session_id = sm::utils::string_to_unsigned_short(session_id);
    } catch (const sm::RuntimeError& e) {
        LOG_DIST_ERROR("Invalid code: {}", e.what());
        m_ui.push_modal_window(ModalWindowJoinGameSessionError, "Invalid code");
        return;
    }

    payload.player_name = g.options.name;
    payload.game_mode = protocol::GameMode(g.options.game_mode);

    networking::Message message {protocol::message::Client_RequestJoinGameSession};

    if (!try_write_message(message, payload)) {
        return;
    }

    if (!try_send_message(message)) {
        return;
    }

    LOG_DEBUG("Requested to join a game session");
    m_ui.push_modal_window(ModalWindowWaitServerAcceptJoinGameSession);
}

void GameScene::client_play_move(protocol::ClockTime time, const std::string& move, bool game_over) {
    assert(m_game_session);

    protocol::Client_PlayMove payload;
    payload.session_id = m_game_session->get_session_id();
    payload.time = time;
    payload.game_over = game_over;
    payload.move = move;

    networking::Message message {protocol::message::Client_PlayMove};

    if (!try_write_message(message, payload)) {
        return;
    }

    try_send_message(message);
}

void GameScene::client_update_turn_time(protocol::ClockTime time) {
    assert(m_game_session);

    protocol::Client_UpdateTurnTime payload;
    payload.session_id = m_game_session->get_session_id();
    payload.time = time;

    networking::Message message {protocol::message::Client_UpdateTurnTime};

    if (!try_write_message(message, payload)) {
        return;
    }

    try_send_message(message);
}

void GameScene::client_resign() {
    assert(m_game_session);

    protocol::Client_Resign payload;
    payload.session_id = m_game_session->get_session_id();

    networking::Message message {protocol::message::Client_Resign};

    if (!try_write_message(message, payload)) {
        return;
    }

    try_send_message(message);
}

void GameScene::client_offer_draw() {
    assert(m_game_session);

    protocol::Client_OfferDraw payload;
    payload.session_id = m_game_session->get_session_id();

    networking::Message message {protocol::message::Client_OfferDraw};

    if (!try_write_message(message, payload)) {
        return;
    }

    try_send_message(message);
}

void GameScene::client_accept_draw() {
    assert(m_game_session);

    protocol::Client_AcceptDraw payload;
    payload.session_id = m_game_session->get_session_id();

    networking::Message message {protocol::message::Client_AcceptDraw};

    if (!try_write_message(message, payload)) {
        return;
    }

    try_send_message(message);
}

void GameScene::client_send_message(const std::string& message_) {
    assert(m_game_session);

    protocol::Client_SendMessage payload;
    payload.session_id = m_game_session->get_session_id();
    payload.message = message_;

    networking::Message message {protocol::message::Client_SendMessage};

    if (!try_write_message(message, payload)) {
        return;
    }

    try_send_message(message);
}

void GameScene::client_rematch() {
    assert(m_game_session);

    protocol::Client_Rematch payload;
    payload.session_id = m_game_session->get_session_id();

    networking::Message message {protocol::message::Client_Rematch};

    if (!try_write_message(message, payload)) {
        return;
    }

    if (!try_send_message(message)) {
        return;
    }

    m_ui.push_modal_window(ModalWindowWaitRemoteRematch);
}

void GameScene::client_cancel_rematch() {
    assert(m_game_session);

    protocol::Client_CancelRematch payload;
    payload.session_id = m_game_session->get_session_id();

    networking::Message message {protocol::message::Client_CancelRematch};

    if (!try_write_message(message, payload)) {
        return;
    }

    try_send_message(message);
}

void GameScene::on_key_released(const sm::KeyReleasedEvent& event) {
    if (event.key == sm::Key::Space) {
        reset_camera_position();
    }
}

void GameScene::on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event) {
    if (event.button == sm::Button::Left) {
        if (m_game_state == GameState::HumanThinking) {
            get_board().user_click_press();
        }
    }
}

void GameScene::on_mouse_button_released(const sm::MouseButtonReleasedEvent& event) {
    if (event.button == sm::Button::Left) {
        if (m_game_state == GameState::HumanThinking) {
            get_board().user_click_release();
        }
    }
}

void GameScene::setup_camera() {
    const auto& g {ctx.global<Global>()};

    m_camera_controller = std::make_shared<PointCameraController>(
        ctx.render_3d()->camera,
        ctx.get_window_width(),
        ctx.get_window_height(),
        glm::vec3(0.0f),
        8.0f,
        47.0f,
        g.options.camera_sensitivity
    );

    ctx.render_3d()->camera_controller = m_camera_controller;

    m_camera_controller->connect_events(ctx);

    m_white_camera_position = m_camera_controller->get_position();
    m_black_camera_position = glm::vec3(m_white_camera_position.x, m_white_camera_position.y, -m_white_camera_position.z);

    m_camera_controller->set_distance_to_point(m_camera_controller->get_distance_to_point() + 1.0f);
    reset_camera_position();
}

void GameScene::setup_skybox(bool reload) {
    ctx.render_3d()->skybox.texture = load_skybox_texture_cubemap(reload);
}

void GameScene::setup_lights() {
    const auto& g {ctx.global<Global>()};

    switch (g.options.skybox) {
        case SkyboxNone:
            ctx.render_3d()->directional_light.direction = glm::normalize(glm::vec3(0.307f, -0.901f, 0.307f));
            ctx.render_3d()->directional_light.ambient_color = glm::vec3(0.1f);
            ctx.render_3d()->directional_light.diffuse_color = glm::vec3(0.5f);
            ctx.render_3d()->directional_light.specular_color = glm::vec3(0.8f);
            break;
        case SkyboxField:
            ctx.render_3d()->directional_light.direction = glm::normalize(glm::vec3(-0.525f, -0.405f, -0.748f));
            ctx.render_3d()->directional_light.ambient_color = glm::vec3(0.25f);
            ctx.render_3d()->directional_light.diffuse_color = glm::vec3(0.7f);
            ctx.render_3d()->directional_light.specular_color = glm::vec3(1.0f);
            break;
        case SkyboxAutumn:
            ctx.render_3d()->directional_light.direction = glm::normalize(glm::vec3(0.370f, -0.925f, -0.092f));
            ctx.render_3d()->directional_light.ambient_color = glm::vec3(0.2f);
            ctx.render_3d()->directional_light.diffuse_color = glm::vec3(0.7f);
            ctx.render_3d()->directional_light.specular_color = glm::vec3(0.9f);
            break;
        case SkyboxSummer:
            ctx.render_3d()->directional_light.direction = glm::normalize(glm::vec3(-0.291f, -0.843f, -0.453f));
            ctx.render_3d()->directional_light.ambient_color = glm::vec3(0.25f);
            ctx.render_3d()->directional_light.diffuse_color = glm::vec3(0.75f);
            ctx.render_3d()->directional_light.specular_color = glm::vec3(1.0f);
            break;
        case SkyboxNight:
            ctx.render_3d()->directional_light.direction = glm::normalize(glm::vec3(0.159f, -0.752f, -0.639f));
            ctx.render_3d()->directional_light.ambient_color = glm::vec3(0.1f);
            ctx.render_3d()->directional_light.diffuse_color = glm::vec3(0.01f);
            ctx.render_3d()->directional_light.specular_color = glm::vec3(0.01f);
            break;
        case SkyboxSunset:
            ctx.render_3d()->directional_light.direction = glm::normalize(glm::vec3(-0.581f, -0.196f, -0.790f));
            ctx.render_3d()->directional_light.ambient_color = glm::vec3(0.2f);
            ctx.render_3d()->directional_light.diffuse_color = glm::vec3(1.0f, 1.0f, 0.9f);
            ctx.render_3d()->directional_light.specular_color = glm::vec3(1.0f);
            break;
        case SkyboxSky:
            ctx.render_3d()->directional_light.direction = glm::normalize(glm::vec3(-0.584f, -0.167f, -0.794f));
            ctx.render_3d()->directional_light.ambient_color = glm::vec3(0.1f);
            ctx.render_3d()->directional_light.diffuse_color = glm::vec3(0.6f, 0.6f, 0.4f);
            ctx.render_3d()->directional_light.specular_color = glm::vec3(1.0f);
            break;
    }
}

void GameScene::load_icons() {
    load_game_icons();

    sm::TextureSpecification specification;
    specification.format = sm::TextureFormat::Rgba8;

    sm::TexturePostProcessing post_processing;
    post_processing.flip = false;

    m_icon_wait = ctx.load_texture(
        "icon_wait"_H,
        ctx.load_texture_data(ctx.path_assets("textures/icons/icon_wait.png"), post_processing),
        specification
    );
}

void GameScene::load_sounds() {
    m_sound_new_game = ctx.load_sound_data(ctx.path_assets("sounds/ui/new_game.ogg"));
    m_sound_game_start = ctx.load_sound_data(ctx.path_assets("sounds/ui/game_start.ogg"));
    m_sound_game_over = ctx.load_sound_data(ctx.path_assets("sounds/ui/game_over.ogg"));
    m_sound_message = ctx.load_sound_data(ctx.path_assets("sounds/ui/message.ogg"));
    m_sound_draw = ctx.load_sound_data(ctx.path_assets("sounds/ui/draw.ogg"));
}

void GameScene::reload_skybox_texture_data() const {
    // Global options must have been set to the desired skybox

    const auto& g {ctx.global<Global>()};

    sm::TexturePostProcessing post_processing;
    post_processing.flip = false;

    if (g.options.texture_quality == TextureQualityHalf) {
        post_processing.size = sm::TextureSize::Half;
    }

    std::string skybox;

    switch (g.options.skybox) {
        case SkyboxNone:
            break;
        case SkyboxField:
            skybox = "field";
            break;
        case SkyboxAutumn:
            skybox = "autumn";
            break;
        case SkyboxSummer:
            skybox = "summer";
            break;
        case SkyboxNight:
            skybox = "night";
            break;
        case SkyboxSunset:
            skybox = "sunset";
            break;
        case SkyboxSky:
            skybox = "sky";
            break;
    }

    if (!skybox.empty()) {
        ctx.reload_texture_data(ctx.path_assets("textures/skybox/" + skybox + "/px.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/skybox/" + skybox + "/nx.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/skybox/" + skybox + "/py.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/skybox/" + skybox + "/ny.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/skybox/" + skybox + "/pz.png"), post_processing);
        ctx.reload_texture_data(ctx.path_assets("textures/skybox/" + skybox + "/nz.png"), post_processing);
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
        case SkyboxSummer:
            return load_or_reload("summer"_H);
        case SkyboxNight:
            return load_or_reload("night"_H);
        case SkyboxSunset:
            return load_or_reload("sunset"_H);
        case SkyboxSky:
            return load_or_reload("sky"_H);
    }

    return {};
}

void GameScene::update_game_state() {
    const auto& g {ctx.global<Global>()};

    switch (m_game_state) {
        case GameState::Ready:
            break;
        case GameState::Start:
            // We only want to actually start the clocks and the game after a short period
            ctx.add_task_delayed([this]() {
                // The state could have changed in the meantime
                if (m_game_state == GameState::Set) {
                    m_game_state = GameState::Go;
                }

                return sm::Task::Result::Done;
            }, 2.0);

            reset_camera_position();
            sm::Ctx::play_audio_sound(m_sound_game_start);
            m_game_state = GameState::Set;

            break;
        case GameState::Set:
            break;
        case GameState::Go:
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

                if (m_game_session) {
                    m_game_session->set_remote_offered_draw(false);
                }

                if (get_board().get_game_over() != GameOver::None) {
                    m_game_state = GameState::Stop;
                } else {
                    m_game_state = GameState::NextTurn;
                }
            }

            break;
        case GameState::Stop:
            switch (g.options.game_type) {
                case GameTypeLocalHumanVsComputer:
                    assert_engine_game_over();
                    break;
                case GameTypeOnline:
                    // The session might have been already destroyed
                    if (m_game_session) {
                        m_game_session->set_remote_offered_draw(false);
                    }
                    break;
            }

            m_ui.push_modal_window(ModalWindowGameOver);
            m_clock.stop();
            m_game_state = GameState::Over;

            sm::Ctx::play_audio_sound(m_sound_game_over);

            break;
        case GameState::Over:
            break;
    }
}

void GameScene::engine_error(const EngineError& e) {
    LOG_DIST_ERROR("Engine error: {}", e.what());
    m_engine.reset();
    m_ui.push_modal_window(ModalWindowEngineError);
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

void GameScene::connection_error(const networking::ConnectionError& e) {
    LOG_DIST_ERROR("Connection error: {}", e.what());

    // To prevent bad states and desynchronizations
    reset_session_and_game();

    m_ui.clear_modal_window(  // The user may already be blocked in a modal window
        ModalWindowWaitServerAcceptGameSession |
        ModalWindowWaitRemoteJoinGameSession |
        ModalWindowWaitServerAcceptJoinGameSession |
        ModalWindowWaitRemoteRematch
    );
    m_ui.push_modal_window(ModalWindowConnectionError);
}

void GameScene::serialization_error(const networking::SerializationError& e) {
    LOG_DIST_CRITICAL("Serialization error: {}", e.what());

    disconnect();

    m_ui.clear_modal_window(  // The user may already be blocked in a modal window
        ModalWindowWaitServerAcceptGameSession |
        ModalWindowWaitRemoteJoinGameSession |
        ModalWindowWaitServerAcceptJoinGameSession |
        ModalWindowWaitRemoteRematch
    );
    m_ui.push_modal_window(ModalWindowConnectionError);
}

bool GameScene::try_write_message(networking::Message& message, auto payload) {
    try {
        message.write(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return false;
    }

    return true;
}

bool GameScene::try_read_message(const networking::Message& message, auto& payload) {
    try {
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return false;
    }

    return true;
}

bool GameScene::try_send_message(const networking::Message& message) {
    auto& g {ctx.global<Global>()};

    try {
        g.client.send_message(message);
    } catch (const networking::ConnectionError& e) {
        connection_error(e);
        return false;
    }

    return true;
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
        switch (g.client.get_connection_state()) {
            case ConnectionState::Disconnected:
                break;
            case ConnectionState::Connecting:
                if (g.client.connection_established()) {
                    client_hello();
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
        case protocol::message::Server_HelloAccept:
            server_hello_accept(message);
            break;
        case protocol::message::Server_HelloReject:
            server_hello_reject(message);
            break;
        case protocol::message::Server_Ping:
            server_ping(message);
            break;
        case protocol::message::Server_AcceptGameSession:
            server_accept_game_session(message);
            break;
        case protocol::message::Server_RejectGameSession:
            server_reject_game_session(message);
            break;
        case protocol::message::Server_AcceptJoinGameSession:
            server_accept_join_game_session(message);
            break;
        case protocol::message::Server_RejectJoinGameSession:
            server_reject_join_game_session(message);
            break;
        case protocol::message::Server_RemoteJoinedGameSession:
            server_remote_joined_game_session(message);
            break;
        case protocol::message::Server_RemoteLeftGameSession:
            server_remote_left_game_session(message);
            break;
        case protocol::message::Server_RemotePlayedMove:
            server_remote_played_move(message);
            break;
        case protocol::message::Server_RemoteResigned:
            server_remote_resigned(message);
            break;
        case protocol::message::Server_RemoteOfferedDraw:
            server_remote_offered_draw(message);
            break;
        case protocol::message::Server_RemoteAcceptedDraw:
            server_remote_accepted_draw(message);
            break;
        case protocol::message::Server_RemoteSentMessage:
            server_remote_sent_message(message);
            break;
        case protocol::message::Server_Rematch:
            server_rematch(message);
            break;
        case protocol::message::Server_CancelRematch:
            server_cancel_rematch(message);
            break;
    }
}

void GameScene::server_hello_accept(const networking::Message& message) {
    protocol::Server_HelloAccept payload;

    if (!try_read_message(message, payload)) {
        return;
    }

    LOG_DEBUG("Server version: {:#06}", payload.version);
}

void GameScene::server_hello_reject(const networking::Message& message) {
    protocol::Server_HelloReject payload;

    if (!try_read_message(message, payload)) {
        return;
    }

    LOG_DIST_ERROR("Server (version {:#06}) rejected the connection: {}", payload.version, protocol::error_code_string(payload.error_code));

    // The server closed us down
    // Prevent showing a connection error
    disconnect();

    m_ui.push_modal_window(ModalWindowServerRejection, protocol::error_code_string(payload.error_code));
}

void GameScene::server_ping(const networking::Message& message) {
    protocol::Server_Ping payload;

    try {
        message.read(payload);
    } catch (const networking::SerializationError& e) {
        serialization_error(e);
        return;
    }

    // TODO anything?
}

void GameScene::server_accept_game_session(const networking::Message& message) {
    protocol::Server_AcceptGameSession payload;

    if (!try_read_message(message, payload)) {
        return;
    }

    m_game_session = GameSession(payload.session_id);

    m_ui.clear_modal_window(ModalWindowWaitServerAcceptGameSession);
    m_ui.push_modal_window(ModalWindowWaitRemoteJoinGameSession);
}

void GameScene::server_reject_game_session(const networking::Message& message) {
    protocol::Server_RejectGameSession payload;

    if (!try_read_message(message, payload)) {
        return;
    }

    m_ui.clear_modal_window(ModalWindowWaitServerAcceptGameSession);
    m_ui.push_modal_window(ModalWindowNewGameSessionError, protocol::error_code_string(payload.error_code));
}

void GameScene::server_accept_join_game_session(const networking::Message& message) {
    protocol::Server_AcceptJoinGameSession payload;

    if (!try_read_message(message, payload)) {
        return;
    }

    m_game_session = GameSession(payload.session_id);
    m_game_session->remote_joined(payload.remote_name);
    m_game_session->set_messages(payload.messages);

    m_game_options.remote_color = PlayerColor(payload.remote_player);
    set_time_control_options(payload.initial_time);

    // This also resets the camera; call it after setting the color
    reset(payload.moves);

    switch (m_game_options.remote_color) {
        case PlayerColorWhite:
            m_clock.set_white_time(payload.remote_time);
            m_clock.set_black_time(payload.time);
            break;
        case PlayerColorBlack:
            m_clock.set_black_time(payload.remote_time);
            m_clock.set_white_time(payload.time);
            break;
    }

    // Unblock from waiting
    m_ui.clear_modal_window(ModalWindowWaitServerAcceptJoinGameSession);

    // The game has previously ended
    if (payload.game_over) {
        m_game_state = GameState::Over;
    }

    // The game could be already over, so don't mess up the state
    if (m_game_state != GameState::Over) {
        m_game_state = GameState::Start;
    }
}

void GameScene::server_reject_join_game_session(const networking::Message& message) {
    protocol::Server_RejectJoinGameSession payload;

    if (!try_read_message(message, payload)) {
        return;
    }

    m_ui.clear_modal_window(ModalWindowWaitServerAcceptJoinGameSession);
    m_ui.push_modal_window(ModalWindowJoinGameSessionError, protocol::error_code_string(payload.error_code));
}

void GameScene::server_remote_joined_game_session(const networking::Message& message) {
    // The user may have left the session in the meantime
    if (!m_game_session) {
        return;
    }

    protocol::Server_RemoteJoinedGameSession payload;

    if (!try_read_message(message, payload)) {
        return;
    }

    m_game_session->remote_joined(payload.remote_name);

    // Unblock from any waiting
    m_ui.clear_modal_window(ModalWindowWaitRemoteJoinGameSession);

    // Remote could be just rejoining an already started game, or the game could have already ended
    if (m_game_state == GameState::Ready) {
        m_game_state = GameState::Start;
    }
}

void GameScene::server_remote_left_game_session(const networking::Message&) {
    // The user may have left the session in the meantime
    if (!m_game_session) {
        return;
    }

    m_game_session->remote_left();
}

void GameScene::server_remote_played_move(const networking::Message& message) {
    // The user may have left the session in the meantime
    if (!m_game_session) {
        return;
    }

    protocol::Server_RemotePlayedMove payload;

    if (!try_read_message(message, payload)) {
        return;
    }

    assert(m_game_state == GameState::RemoteThinking);

    // This ensures that clock remain in sync
    switch (get_board().get_player_color()) {
        case PlayerColorWhite:
            m_clock.set_white_time(payload.time);
            break;
        case PlayerColorBlack:
            m_clock.set_black_time(payload.time);
            break;
    }

    play_move(payload.move);
}

void GameScene::server_remote_resigned(const networking::Message&) {
    // The user may have left the session in the meantime
    if (!m_game_session) {
        return;
    }

    resign(PlayerColor(m_game_options.remote_color));
}

void GameScene::server_remote_offered_draw(const networking::Message&) {
    // The user may have left the session in the meantime
    if (!m_game_session) {
        return;
    }

    m_game_session->set_remote_offered_draw(true);

    sm::Ctx::play_audio_sound(m_sound_draw);
}

void GameScene::server_remote_accepted_draw(const networking::Message&) {
    // The user may have left the session in the meantime
    if (!m_game_session) {
        return;
    }

    // The remote has accepted our draw, so we "accept" it again
    accept_draw();
}

void GameScene::server_remote_sent_message(const networking::Message& message) {
    // The user may have left the session in the meantime
    if (!m_game_session) {
        return;
    }

    protocol::Server_RemoteSentMessage payload;

    if (!try_read_message(message, payload)) {
        return;
    }

    m_game_session->remote_sent_message(payload.message);

    sm::Ctx::play_audio_sound(m_sound_message);
}

void GameScene::server_rematch(const networking::Message& message) {
    // The user may have left the session in the meantime
    if (!m_game_session) {
        return;
    }

    protocol::Server_Rematch payload;

    if (!try_read_message(message, payload)) {
        return;
    }

    m_game_options.remote_color = PlayerColor(payload.remote_player);
    set_time_control_options(payload.initial_time);  // Also reset the time, just to be sure...

    // Call this after setting the color
    reset();

    m_game_state = GameState::Start;

    // Unblock from waiting
    m_ui.clear_modal_window(ModalWindowWaitRemoteRematch);
}

void GameScene::server_cancel_rematch(const networking::Message&) {
    // The user may have left the session in the meantime
    if (!m_game_session) {
        return;
    }

    // Unblock from waiting
    m_ui.clear_modal_window(ModalWindowWaitRemoteRematch);
}
