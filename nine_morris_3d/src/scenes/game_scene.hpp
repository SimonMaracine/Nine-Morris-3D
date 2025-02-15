#pragma once

#include <string>
#include <optional>
#include <filesystem>
#include <vector>
#include <utility>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <networking/client.hpp>

#include "engines/engine.hpp"
#include "game/board.hpp"
#include "point_camera_controller.hpp"
#include "ui.hpp"
#include "clock.hpp"
#include "moves_list.hpp"
#include "game_session.hpp"
#include "game_options.hpp"
#include "saved_games.hpp"
#include "game_analysis.hpp"
#include "player_color.hpp"

enum class GameState {
    Ready,
    Start,
    Set,
    Go,
    NextTurn,
    HumanThinking,
    ComputerStartThinking,
    ComputerThinking,
    RemoteThinking,
    FinishTurn,
    Stop,
    Over,
    Analyze
};

// Type of player
enum class GamePlayer {
    Human,
    Computer,
    Remote
};

// Moves with player's time after the move attached
using TimedMoves = std::vector<std::pair<std::string, Clock::Time>>;

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
    virtual const BoardObj& get_board() const = 0;
    virtual GamePlayer get_player_type() const = 0;
    virtual std::string get_setup_position() const = 0;
    virtual void reset(const TimedMoves& moves = {}) = 0;
    virtual void reset_board(const std::string& string) = 0;
    virtual bool second_player_starting() = 0;
    virtual Clock::Time clock_time(int time_enum) = 0;
    virtual void set_time_control_options(Clock::Time time) = 0;
    virtual void play_move(const std::string& string) = 0;
    virtual void timeout(PlayerColor color) = 0;
    virtual void resign(PlayerColor color) = 0;
    virtual void accept_draw() = 0;
    virtual void time_control_options_window() = 0;
    virtual void start_engine() = 0;
    virtual void load_game_icons() = 0;
    virtual void reload_scene_texture_data() const = 0;
    virtual void reload_and_set_scene_textures() = 0;
    virtual std::filesystem::path saved_games_file_path() const = 0;

    std::shared_ptr<PointCameraController> get_camera_controller() const { return m_camera_controller; }
    const std::unique_ptr<Engine>& get_engine() const { return m_engine; }
    GameState& get_game_state() { return m_game_state; }
    GameOptions& get_game_options() { return m_game_options; }
    std::optional<GameSession>& get_game_session() { return m_game_session; }
    std::optional<GameAnalysis>& get_game_analysis() { return m_game_analysis; }
    const Clock& get_clock() const { return m_clock; }
    MovesList& get_moves_list() { return m_moves_list; }
    const SavedGames& get_saved_games() const { return m_saved_games; }
    std::shared_ptr<sm::GlTexture> get_icon_wait() const { return m_icon_wait; }
    std::shared_ptr<sm::GlTexture> get_icon_white() const { return m_icon_white; }
    std::shared_ptr<sm::GlTexture> get_icon_black() const { return m_icon_black; }

    void reload_and_set_skybox();
    void reload_and_set_textures();

    bool resign_available() const;
    PlayerColor resign_player() const;
    bool offer_draw_available() const;
    bool accept_draw_available() const;
    bool game_in_progress() const;

    void reset(const std::string& string, const TimedMoves& moves = {});
    void reset_camera_position();
    void analyze_game(std::size_t index);
    void resign_leave_session_and_reset();

    void connect(const std::string& address, std::uint16_t port);
    void connect(const std::string& address, const std::string& port);
    void connect();
    void disconnect();
    void client_hello();
    void client_ping();
    void client_request_game_session();
    void client_leave_game_session();
    void client_request_join_game_session(const std::string& session_id);
    void client_play_move(const std::string& move, protocol::ClockTime time, bool game_over);
    void client_update_turn_time(protocol::ClockTime time);
    void client_timeout();
    void client_resign();
    void client_offer_draw();
    void client_accept_draw();
    void client_send_message(const std::string& message_);
    void client_rematch();
    void client_cancel_rematch();
protected:
    void on_key_released(const sm::KeyReleasedEvent& event);
    void on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event);
    void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);

    void setup_camera();
    void setup_skybox(bool reload = false);
    void setup_lights();

    void load_icons();
    void load_sounds();
    void reload_skybox_texture_data() const;
    std::shared_ptr<sm::GlTextureCubemap> load_skybox_texture_cubemap(bool reload = false) const;

    void update_game_state();
    void game_state_start();
    void game_state_go();
    void game_state_next_turn();
    void game_state_computer_start_thinking();
    void game_state_computer_thinking();
    void game_state_finish_turn();
    void game_state_stop();

    void engine_error(const EngineError& e);
    void stop_engine();
    void assert_engine_game_over();

    void connection_error(const networking::ConnectionError& e);
    void serialization_error(const networking::SerializationError& e);
    bool try_write_message(networking::Message& message, auto payload);
    bool try_read_message(const networking::Message& message, auto& payload);
    bool try_send_message(const networking::Message& message);
    void reset_game_if_session();
    void update_connection_state();
    void handle_message(const networking::Message& message);
    void server_hello_accept(const networking::Message& message);
    void server_hello_reject(const networking::Message& message);
    void server_ping(const networking::Message& message);
    void server_accept_game_session(const networking::Message& message);
    void server_reject_game_session(const networking::Message& message);
    void server_accept_join_game_session(const networking::Message& message);
    void server_reject_join_game_session(const networking::Message& message);
    void server_remote_joined_game_session(const networking::Message& message);
    void server_remote_left_game_session(const networking::Message& message);
    void server_remote_played_move(const networking::Message& message);
    void server_remote_timed_out(const networking::Message& message);
    void server_remote_resigned(const networking::Message& message);
    void server_remote_offered_draw(const networking::Message& message);
    void server_remote_accepted_draw(const networking::Message& message);
    void server_remote_sent_message(const networking::Message& message);
    void server_rematch(const networking::Message& message);
    void server_cancel_rematch(const networking::Message& message);

    std::shared_ptr<PointCameraController> m_camera_controller;
    std::unique_ptr<Engine> m_engine;
    Ui m_ui;

    std::shared_ptr<sm::GlTexture> m_icon_wait;
    std::shared_ptr<sm::GlTexture> m_icon_white;
    std::shared_ptr<sm::GlTexture> m_icon_black;

    std::shared_ptr<sm::SoundData> m_sound_new_game;
    std::shared_ptr<sm::SoundData> m_sound_game_start;
    std::shared_ptr<sm::SoundData> m_sound_game_over;
    std::shared_ptr<sm::SoundData> m_sound_message;
    std::shared_ptr<sm::SoundData> m_sound_draw;

    GameState m_game_state {GameState::Ready};
    glm::vec3 m_white_camera_position {};
    glm::vec3 m_black_camera_position {};
    std::optional<GameSession> m_game_session;
    GameOptions m_game_options;
    Clock m_clock;
    MovesList m_moves_list;
    SavedGames m_saved_games;  // All saved games
    SavedGame m_current_game;  // Used to save the game after is over
    std::optional<GameAnalysis> m_game_analysis;
};
