// #pragma once

// #include <nine_morris_3d_engine/nine_morris_3d.hpp>

// #include "game/jump_variant_board.hpp"
// #include "scenes/game_scene.hpp"

// class JumpVariantScene : public GameScene {
// public:
//     explicit JumpVariantScene(sm::Ctx& ctx)
//         : GameScene(ctx) {}

//     SM_SCENE_NAME("jump_variant")

//     void on_start() override;
//     void on_stop() override;
//     void on_update() override;
//     void on_fixed_update() override;
//     void on_imgui_update() override;

//     BoardObj& get_board() override;
//     void play_move_on_board(const std::string& string) override;
// private:
//     void on_key_released(const sm::KeyReleasedEvent& event);
//     void on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event);
//     void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);

//     JumpVariantBoard setup_renderables();

//     JumpVariantBoard m_board;
//     TurnIndicator m_turn_indicator;
//     Timer m_timer;
// };
