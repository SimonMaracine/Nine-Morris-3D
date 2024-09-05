#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/standard_game_board.hpp"
#include "scenes/game_scene.hpp"
#include "turn_indicator.hpp"
#include "timer.hpp"

class StandardGameScene : public GameScene {
public:
    explicit StandardGameScene(sm::Ctx& ctx)
        : GameScene(ctx) {}

    SM_SCENE_NAME("standard_game")

    void on_start() override;
    void on_stop() override;
    void on_update() override;
    void on_fixed_update() override;
    void on_imgui_update() override;

    Board& get_board() override;
private:
    void on_key_released(const sm::KeyReleasedEvent& event);
    void on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event);
    void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);

    void setup_turn_indicator();
    void setup_timer();
    void setup_renderables();

    StandardGameBoard m_board;
    TurnIndicator m_turn_indicator;
    Timer m_timer;
};
