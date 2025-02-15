#pragma once

#include <string>
#include <functional>
#include <deque>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "options.hpp"

class GameScene;

enum ModalWindow : unsigned int {
    ModalWindowNone = 0,
    ModalWindowGeneralPlay = 1u << 0,
    ModalWindowOnlinePlay = 1u << 1,
    ModalWindowAbout = 1u << 2,
    ModalWindowGameOver = 1u << 3,
    ModalWindowGameOptions = 1u << 4,
    ModalWindowAnalyzeGames = 1u << 5,
    ModalWindowEngineError = 1u << 6,
    ModalWindowConnectionError = 1u << 7,
    ModalWindowServerRejection = 1u << 8,
    ModalWindowNewGameSessionError = 1u << 9,
    ModalWindowJoinGameSessionError = 1u << 10,
    ModalWindowWaitServerAcceptGameSession = 1u << 11,
    ModalWindowWaitRemoteJoinGameSession = 1u << 12,
    ModalWindowWaitServerAcceptJoinGameSession = 1u << 13,
    ModalWindowWaitRemoteRematch = 1u << 14,
    ModalWindowRulesNineMensMorris = 1u << 15,
    ModalWindowRulesTwelveMensMorris = 1u << 16
};

class Ui {
public:
    void initialize(sm::Ctx& ctx);
    void update(sm::Ctx& ctx, GameScene& game_scene);
    void push_modal_window(ModalWindow window, const std::string& string = {});
    void clear_modal_window();
    void clear_modal_window(unsigned int windows);
    ModalWindow get_modal_window() const;

    static float rem(float size);

    void set_loading_skybox_done() { m_loading_skybox = false; }
    bool get_show_information() const { return m_show_information; }
private:
    void main_menu_bar(sm::Ctx& ctx, GameScene& game_scene);
    void game_window(sm::Ctx& ctx, GameScene& game_scene);
    void before_game_window(sm::Ctx& ctx, GameScene& game_scene);
    void before_game_local_window(sm::Ctx& ctx, GameScene& game_scene);
    void before_game_online_window(sm::Ctx& ctx, GameScene& game_scene);
    void during_game_window(GameScene& game_scene);
    void analyze_game_window(GameScene& game_scene);
    void about_window();
    void general_play_window();
    void online_play_window();
    void game_over_window(GameScene& game_scene);
    void game_options_window(sm::Ctx& ctx, GameScene& game_scene);
    void analyze_games_window(GameScene& game_scene);
    void engine_error_window();
    void connection_error_window();
    void server_rejection_window(const std::string& string);
    void new_game_session_error_window(const std::string& string);
    void join_game_session_error_window(const std::string& string);
    void wait_server_accept_game_session_window(GameScene& game_scene);
    void wait_remote_join_game_session_window(GameScene& game_scene);
    void wait_server_accept_join_game_session_window(GameScene& game_scene);
    void wait_remote_rematch_window(GameScene& game_scene);
    void rules_nine_mens_morris_window();
    void rules_twelve_mens_morris_window();
    void modal_window(const char* title, std::function<bool()>&& contents);
    void modal_window_ok(const char* title, std::function<void()>&& contents);
    void modal_window_ok_size(const char* title, std::function<void()>&& contents, glm::vec2 size = {});
    void modal_window_ok_size_constraints(const char* title, std::function<void()>&& contents, glm::vec2 min_size = {}, glm::vec2 max_size = {});

    static void set_scale(sm::Ctx& ctx, int scale);
    static void set_scale_task(sm::Ctx& ctx, int scale);
    static void set_anti_aliasing_task(sm::Ctx& ctx, int samples);
    static void set_shadow_quality_task(sm::Ctx& ctx, int size);
    static void set_anisotropic_filtering_task(sm::Ctx& ctx, GameScene& game_scene);
    static void create_font(sm::Ctx& ctx, int scale);
    static void set_style();

    bool join_game_available(GameScene& game_scene) const;

    // Multiple modal windows may request attention at a time; put them in a queue
    std::deque<std::pair<ModalWindow, std::string>> m_modal_window_queue;

    // Local options data
    // When changed, update the options from the global data
    Options m_options;

    char m_session_id[5 + 1] {};

    bool m_loading_skybox {false};  // This is needed, because selecting a skybox doesn't close the interface
    bool m_show_information {false};
};
