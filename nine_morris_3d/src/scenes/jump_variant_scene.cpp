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

    m_board.update(ctx, cast_mouse_ray(ctx, m_cam), m_cam_controller.get_position());

    const auto& g {ctx.global<Global>()};

    if (!g.options.hide_turn_indicator) {
        m_turn_indicator.update(ctx, static_cast<TurnIndicatorType>(m_board.get_turn()));
    }

    m_timer.update();

    if (!g.options.hide_timer) {
        m_timer.render(ctx);
    }

    ctx.shadow(m_shadow_box);
}

void JumpVariantScene::on_fixed_update() {
    GameScene::on_fixed_update();

    m_board.update_movement();
}

void JumpVariantScene::on_imgui_update() {
    GameScene::on_imgui_update();

    m_board.debug();
}

BoardObj& JumpVariantScene::get_board() {
    return m_board;
}

void JumpVariantScene::on_key_released(const sm::KeyReleasedEvent& event) {
    if (event.key == sm::Key::Space) {
        m_cam_controller.go_towards_position(m_default_camera_position);
    }
}

void JumpVariantScene::on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        m_board.user_click_press();
    }
}

void JumpVariantScene::on_mouse_button_released(const sm::MouseButtonReleasedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        m_board.user_click_release();
    }
}

void JumpVariantScene::setup_turn_indicator() {
    sm::TextureSpecification specification;
    specification.format = sm::TextureFormat::Rgba8;

    const auto white_texture {ctx.load_texture("white_indicator"_H, ctx.get_texture_data("white_indicator.png"_H), specification)};
    const auto black_texture {ctx.load_texture("black_indicator"_H, ctx.get_texture_data("black_indicator.png"_H), specification)};

    m_turn_indicator = TurnIndicator(white_texture, black_texture);
}

void JumpVariantScene::setup_timer() {
    sm::FontSpecification specification;
    specification.size_height = 90.0f;
    specification.bitmap_size = 512;

    const auto font {ctx.load_font(
        "open_sans"_H,
        ctx.path_assets("fonts/OpenSans/OpenSans-Regular.ttf"),
        specification,
        [](sm::Font* font) {
            font->begin_baking();
            font->bake_ascii();
            font->end_baking();
        }
    )};

    m_timer = Timer(font);
}

void JumpVariantScene::setup_renderables() {
    const auto renderable_board {setup_board()};
    const auto renderable_board_paint {setup_board_paint()};
    const auto renderable_nodes {setup_nodes(JumpVariantBoard::NODES)};
    const auto renderable_white_pieces {setup_white_pieces(JumpVariantBoard::PIECES / 2)};
    const auto renderable_black_pieces {setup_black_pieces(JumpVariantBoard::PIECES / 2)};

    m_board = JumpVariantBoard(
        renderable_board,
        renderable_board_paint,
        renderable_nodes,
        renderable_white_pieces,
        renderable_black_pieces,
        [this](const JumpVariantBoard::Move&) {
            if (!m_game_started) {
                m_timer.start();

                m_game_started = true;
            }

            if (m_board.get_game_over() != GameOver::None) {
                m_timer.stop();
            }
        }
    );
}
