#pragma once

#include <string>
#include <functional>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "options.hpp"

class GameScene;

enum class PopupWindow {
    None,
    About,
    RulesStandardGame,
    RulesJumpVariant,
    RulesJumpPlusVariant,
    ComputerAi
};

class Ui {
public:
    bool get_show_information() const { return m_show_information; }

    void set_popup_window(PopupWindow window) { m_current_popup_window = window; }

    void initialize(sm::Ctx& ctx);
    void update(sm::Ctx& ctx, GameScene& game_scene);
private:
    void main_menu_bar(sm::Ctx& ctx, GameScene& game_scene);
    void about_window();
    void rules_standard_game_window();
    void rules_jump_variant_window();
    void rules_jump_plus_variant_window();
    void computer_ai_window();
    void wrapped_text_window(const char* title, const char* text);
    void generic_window(const char* title, std::function<void()>&& contents, std::function<void()>&& on_ok = []() {});

    void set_scale_task(sm::Ctx& ctx, int scale);
    void set_scale(sm::Ctx& ctx, int scale);
    void set_anti_aliasing_task(sm::Ctx& ctx, int samples);
    void set_shadow_quality_task(sm::Ctx& ctx, int size);
    void create_font(sm::Ctx& ctx, int scale);
    void set_style();
    static float rem(float size);

    // Local options data
    // When changed, update the options from the global data
    Options m_options;

    std::string m_last_saved_game_date {"No Date"};
    PopupWindow m_current_popup_window {PopupWindow::None};
    bool m_loading_skybox {false};
    bool m_show_information {false};
    bool m_can_undo {false};
    bool m_can_redo {false};
};
