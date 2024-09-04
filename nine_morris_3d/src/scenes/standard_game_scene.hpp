#pragma once

#include <vector>

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

    void load_and_set_skybox() override;
    void load_and_set_board_paint_texture() override;
private:
    void on_key_released(const sm::KeyReleasedEvent& event);
    void on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event);
    void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);

    void setup_turn_indicator();
    void setup_timer();
    void setup_renderables();
    sm::Renderable setup_board() const;
    sm::Renderable setup_board_paint() const;
    std::vector<sm::Renderable> setup_nodes() const;
    std::vector<sm::Renderable> setup_white_pieces() const;
    std::vector<sm::Renderable> setup_black_pieces() const;

    StandardBoard m_board;
    TurnIndicator m_turn_indicator;
    Timer m_timer;
};
