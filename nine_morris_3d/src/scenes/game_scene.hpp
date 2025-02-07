#pragma once

#include <string>
#include <memory>
#include <optional>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <networking/client.hpp>

#include "engines/engine.hpp"
#include "game/board.hpp"
#include "point_camera_controller.hpp"
#include "ui.hpp"
#include "clock.hpp"
#include "move_list.hpp"
#include "game_session.hpp"
#include "game_options.hpp"

enum class GameState {
    Ready,
    Start,
    NextTurn,
    HumanThinking,
    ComputerStartThinking,
    ComputerThinking,
    RemoteThinking,
    FinishTurn,
    Stop,
    Over
};

// Type of player
enum class GamePlayer {
    Human,
    Computer,
    Remote
};

// Base class for scenes representing games
class GameScene : public sm::ApplicationScene {
public:
    explicit GameScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx) {}

    void on_start() override;
    void on_stop() override;
    void on_update() override;
    void on_fixed_update() override;
    void on_imgui_update() override;

    virtual void scene_setup() = 0;
    virtual void scene_update() = 0;
    virtual void scene_fixed_update() = 0;
    virtual void scene_imgui_update() = 0;

    virtual BoardObj& get_board() = 0;
    virtual GamePlayer get_player_type() const = 0;
    virtual std::string get_setup_position() const = 0;
    virtual void reset(const std::vector<std::string>& moves = {}) = 0;
    virtual void reset(const std::string& string, const std::vector<std::string>& moves = {}) = 0;
    virtual void play_move(const std::string& string) = 0;
    virtual void timeout(PlayerColor color) = 0;
    virtual void resign(PlayerColor color) = 0;
    virtual void accept_draw() = 0;
    virtual void time_control_options_window() = 0;

    PointCameraController& get_camera_controller() { return m_camera_controller; }
    GameState& get_game_state() { return m_game_state; }
    GameOptions& get_game_options() { return m_game_options; }
    std::optional<GameSession>& get_game_session() { return m_game_session; }
    const Clock& get_clock() const { return m_clock; }
    const MoveList& get_move_list() const { return m_move_list; }
    const std::unique_ptr<Engine>& get_engine() const { return m_engine; }
    std::shared_ptr<sm::GlTexture> get_icon_wait() const { return m_icon_wait; }
    std::shared_ptr<sm::GlTexture> get_icon_white() const { return m_icon_white; }
    std::shared_ptr<sm::GlTexture> get_icon_black() const { return m_icon_black; }

    virtual void load_game_icons() = 0;
    virtual void reload_scene_texture_data() const = 0;
    virtual void reload_and_set_scene_textures() = 0;
    void reload_and_set_skybox();
    void reload_and_set_textures();

    void reset_camera_position();

    virtual void start_engine() = 0;

    void connect(const std::string& address, std::uint16_t port, bool reconnect = false);
    void connect(const std::string& address, const std::string& port, bool reconnect = false);
    void client_request_game_session();
    void client_leave_game_session();
    void client_request_join_game_session(const std::string& session_id);
    void client_play_move(protocol::ClockTime time, const std::string& move);
    void client_update_turn_time(protocol::ClockTime time);
    void client_resign();
    void client_offer_draw();
    void client_accept_draw();
    void client_send_message(const std::string& message_);
protected:
    void on_window_resized(const sm::WindowResizedEvent& event);
    void on_key_released(const sm::KeyReleasedEvent& event);
    void on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event);
    void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);

    void setup_camera();
    void setup_skybox(bool reload = false);
    void setup_lights();

    void load_icons();
    void reload_skybox_texture_data() const;
    std::shared_ptr<sm::GlTextureCubemap> load_skybox_texture_cubemap(bool reload = false) const;

    void update_game_state();

    void engine_error(const EngineError& e);
    void stop_engine();
    void assert_engine_game_over();

    void connection_error(const networking::ConnectionError& e);
    void serialization_error(const networking::SerializationError& e);
    void reset_session_and_game();
    void update_connection_state();
    void handle_message(const networking::Message& message);
    void client_ping();
    void server_ping(const networking::Message& message);
    void server_accept_game_session(const networking::Message& message);
    void server_reject_game_session(const networking::Message& message);
    void server_accept_join_game_session(const networking::Message& message);
    void server_reject_join_game_session(const networking::Message& message);
    void server_remote_joined_game_session(const networking::Message& message);
    void server_remote_left_game_session(const networking::Message& message);
    void server_remote_played_move(const networking::Message& message);
    void server_remote_resigned(const networking::Message& message);
    void server_remote_offered_draw(const networking::Message& message);
    void server_remote_accepted_draw(const networking::Message& message);
    void server_remote_sent_message(const networking::Message& message);

    sm::Camera m_camera;
    sm::Camera2D m_camera_2d;
    sm::DirectionalLight m_directional_light;
    sm::ShadowBox m_shadow_box;
    sm::Skybox m_skybox;
    PointCameraController m_camera_controller;
    Ui m_ui;

    std::shared_ptr<sm::GlTexture> m_icon_wait;
    std::shared_ptr<sm::GlTexture> m_icon_white;
    std::shared_ptr<sm::GlTexture> m_icon_black;

    GameState m_game_state {GameState::Ready};
    glm::vec3 m_white_camera_position {};
    glm::vec3 m_black_camera_position {};
    std::optional<GameSession> m_game_session;
    GameOptions m_game_options;
    Clock m_clock;
    MoveList m_move_list;
    std::unique_ptr<Engine> m_engine;
};
