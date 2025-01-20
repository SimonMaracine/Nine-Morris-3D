#pragma once

#include <string>
#include <functional>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "options.hpp"

class GameScene;

enum class PopupWindow {
    None,
    About,
    GameOver,
    GameOptions,
    RulesNineMensMorris
};

class Ui {
public:
    bool get_show_information() const { return m_show_information; }

    void set_popup_window(PopupWindow window) { m_current_popup_window = window; }
    void set_loading_skybox_done() { m_loading_skybox = false; }

    void initialize(sm::Ctx& ctx);
    void update(sm::Ctx& ctx, GameScene& game_scene);
private:
    void main_menu_bar(sm::Ctx& ctx, GameScene& game_scene);
    void game_window(GameScene& game_scene);
    void game_window_before_game(GameScene& game_scene);
    void game_window_during_game(GameScene& game_scene);
    void about_window();
    void game_over_window(GameScene& game_scene);
    void game_options_window(GameScene& game_scene);
    void rules_nine_mens_morris_window();
    void wrapped_text_window(const char* title, const char* text);
    void generic_window(const char* title, std::function<void()>&& contents, std::function<void()>&& on_ok = []() {});

    static void set_scale(sm::Ctx& ctx, int scale);
    static void set_scale_task(sm::Ctx& ctx, int scale);
    static void set_anti_aliasing_task(sm::Ctx& ctx, int samples);
    static void set_shadow_quality_task(sm::Ctx& ctx, int size);
    static void set_anisotropic_filtering_task(sm::Ctx& ctx, GameScene& game_scene);
    static void create_font(sm::Ctx& ctx, int scale);
    static void set_style();
    static float rem(float size);

    // Local options data
    // When changed, update the options from the global data
    Options m_options;

    PopupWindow m_current_popup_window {PopupWindow::None};
    bool m_loading_skybox {false};  // This is needed, because selecting a skybox doesn't close the interface
    bool m_show_information {false};
};
