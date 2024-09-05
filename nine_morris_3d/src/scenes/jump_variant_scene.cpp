#include "scenes/jump_variant_scene.hpp"

#include <nine_morris_3d_engine/external/resmanager.h++>

#include "game/ray.hpp"
#include "global.hpp"

void JumpVariantScene::on_start() {
    GameScene::on_start();

    ctx.connect_event<sm::KeyReleasedEvent, &JumpVariantScene::on_key_released>(this);
    ctx.connect_event<sm::MouseButtonPressedEvent, &JumpVariantScene::on_mouse_button_pressed>(this);
    ctx.connect_event<sm::MouseButtonReleasedEvent, &JumpVariantScene::on_mouse_button_released>(this);

    setup_turn_indicator();
    setup_timer();
    setup_renderables();
}

void JumpVariantScene::on_stop() {
    GameScene::on_stop();
}

void JumpVariantScene::on_update() {
    GameScene::on_update();

    // sm::listener::set_position(cam_controller.get_position());  // TODO

    // m_board.update(ctx, cast_mouse_ray(ctx, m_cam), m_cam_controller.get_position());

    const auto& g {ctx.global<Global>()};

    if (!g.options.hide_turn_indicator) {
        // m_turn_indicator.update(ctx, static_cast<TurnIndicatorType>(m_board.get_turn()));
    }

    m_timer.update();

    if (!g.options.hide_timer) {
        m_timer.render(ctx);
    }

    ctx.shadow(m_shadow_box);
}

void JumpVariantScene::on_fixed_update() {
    GameScene::on_fixed_update();

    // m_board.update_movement();
}

void JumpVariantScene::on_imgui_update() {
    GameScene::on_imgui_update();

    // m_board.debug();
}

Board& JumpVariantScene::get_board() {

}

void JumpVariantScene::on_key_released(const sm::KeyReleasedEvent& event) {

}

void JumpVariantScene::on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event) {

}

void JumpVariantScene::on_mouse_button_released(const sm::MouseButtonReleasedEvent& event) {

}

void JumpVariantScene::setup_turn_indicator() {

}

void JumpVariantScene::setup_timer() {

}

void JumpVariantScene::setup_renderables() {

}
