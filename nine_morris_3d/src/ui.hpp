#pragma once

#include <string>
#include <functional>
#include <deque>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "options.hpp"
#include "player_color.hpp"

class GameScene;

enum ModalWindow : unsigned int {
    ModalWindowNone = 0,
    ModalWindowAbout = 1u << 0,
    ModalWindowGameOver = 1u << 1,
    ModalWindowGameOptions = 1u << 2,
    ModalWindowEngineError = 1u << 3,
    ModalWindowConnectionError = 1u << 4,
    ModalWindowServerRejection = 1u << 5,
    ModalWindowNewGameSessionError = 1u << 6,
    ModalWindowJoinGameSessionError = 1u << 7,
    ModalWindowWaitServerAcceptGameSession = 1u << 8,
    ModalWindowWaitRemoteJoinGameSession = 1u << 9,
    ModalWindowWaitServerAcceptJoinGameSession = 1u << 10,
    ModalWindowWaitRemoteRematch = 1u << 11,
    ModalWindowRulesNineMensMorris = 1u << 12
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
    void about_window();
    void game_over_window(GameScene& game_scene);
    void game_options_window(sm::Ctx& ctx, GameScene& game_scene);
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
    void wrapped_text_modal_window(const char* title, const char* text);
    void generic_modal_window_ok(
        const char* title,
        std::function<void()>&& contents,
        std::function<void()>&& on_ok = []() {},
        glm::vec2 size = {}
    );
    void generic_modal_window(const char* title, std::function<bool()>&& contents);

    static void set_scale(sm::Ctx& ctx, int scale);
    static void set_scale_task(sm::Ctx& ctx, int scale);
    static void set_anti_aliasing_task(sm::Ctx& ctx, int samples);
    static void set_shadow_quality_task(sm::Ctx& ctx, int size);
    static void set_anisotropic_filtering_task(sm::Ctx& ctx, GameScene& game_scene);
    static void create_font(sm::Ctx& ctx, int scale);
    static void set_style();

    static bool resign_available(GameScene& game_scene);
    static PlayerColor resign_player(GameScene& game_scene);
    static bool offer_draw_available(GameScene& game_scene);
    static bool accept_draw_available(GameScene& game_scene);
    bool join_game_available(GameScene& game_scene);

    // Multiple modal windows may request attention at a time; put them in a queue
    std::deque<std::pair<ModalWindow, std::string>> m_modal_window_queue;

    // Local options data
    // When changed, update the options from the global data
    Options m_options;

    char m_session_id[5 + 1] {};

    bool m_loading_skybox {false};  // This is needed, because selecting a skybox doesn't close the interface
    bool m_show_information {false};
};
