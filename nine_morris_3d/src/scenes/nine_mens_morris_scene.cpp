#include "scenes/nine_mens_morris_scene.hpp"

#include "game/ray.hpp"
#include "global.hpp"
#include "muhle_engine.hpp"

void StandardGameScene::on_start() {
    GameScene::on_start();

    ctx.connect_event<sm::KeyReleasedEvent, &StandardGameScene::on_key_released>(this);
    ctx.connect_event<sm::MouseButtonPressedEvent, &StandardGameScene::on_mouse_button_pressed>(this);
    ctx.connect_event<sm::MouseButtonReleasedEvent, &StandardGameScene::on_mouse_button_released>(this);

    // m_turn_indicator = setup_turn_indicator();
    // m_timer = setup_timer();
    m_board = setup_renderables();
}

void StandardGameScene::on_stop() {
    GameScene::on_stop();
}

void StandardGameScene::on_update() {
    GameScene::on_update();

    // sm::listener::set_position(cam_controller.get_position());  // TODO

    m_board.update(ctx, cast_mouse_ray(ctx, m_cam), m_cam_controller.get_position());
    update_game_state();

    const auto& g {ctx.global<Global>()};

    // if (!g.options.hide_turn_indicator) {
    //     m_turn_indicator.update(ctx, static_cast<TurnIndicatorType>(m_board.get_turn()));
    // }

    // m_timer.update();

    // if (!g.options.hide_timer) {
    //     m_timer.render(ctx);
    // }

    ctx.shadow(m_shadow_box);
}

void StandardGameScene::on_fixed_update() {
    GameScene::on_fixed_update();

    m_board.update_movement();
}

void StandardGameScene::on_imgui_update() {
    GameScene::on_imgui_update();

    m_board.debug();
}

BoardObj& StandardGameScene::get_board() {
    return m_board;
}

void StandardGameScene::play_move_on_board(const std::string& string) {
    const StandardGameBoard::Move move {StandardGameBoard::move_from_string(string)};

    switch (move.type) {
        case StandardGameBoard::MoveType::Place:
            m_board.place_piece(move.place.place_index);
            break;
        case StandardGameBoard::MoveType::PlaceTake:
            m_board.place_take_piece(move.place_take.place_index, move.place_take.take_index);
            break;
        case StandardGameBoard::MoveType::Move:
            m_board.move_piece(move.move.source_index, move.move.destination_index);
            break;
        case StandardGameBoard::MoveType::MoveTake:
            m_board.move_take_piece(move.move_take.source_index, move.move_take.destination_index, move.move_take.take_index);
            break;
    }
}

void StandardGameScene::on_key_released(const sm::KeyReleasedEvent& event) {
    if (event.key == sm::Key::Space) {
        m_cam_controller.go_towards_position(m_default_camera_position);
    }
}

void StandardGameScene::on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        // if (m_game_state == GameState::HumanMakeMove) {
        //     m_board.user_click_press();
        // }
    }
}

void StandardGameScene::on_mouse_button_released(const sm::MouseButtonReleasedEvent& event) {
    if (event.button == sm::MouseButton::Left) {
        // if (m_game_state == GameState::HumanMakeMove) {
        //     m_board.user_click_release();
        // }
    }
}

StandardGameBoard StandardGameScene::setup_renderables() {
    return StandardGameBoard(
        setup_board(),
        setup_board_paint(),
        setup_nodes(StandardGameBoard::NODES),
        setup_white_pieces(StandardGameBoard::PIECES / 2),
        setup_black_pieces(StandardGameBoard::PIECES / 2),
        [this](const StandardGameBoard::Move& move) {
            // if (!m_game_started) {
            //     // m_timer.start();

            //     m_game_started = true;
            // }

            // m_game_state = GameState::NextPlayer;

            if (m_board.get_game_over() != GameOver::None) {
                // m_timer.stop();
                m_ui.set_popup_window(PopupWindow::GameOver);

                m_game_state = GameState::Over;
            }

            switch (m_board.get_turn()) {
                case Player::White:
                    if (m_player_black == GamePlayer::Human) {
                        // muhle_engine::send_message("move " + StandardGameBoard::string_from_move(move) + '\n');
                    }
                    break;
                case Player::Black:
                    if (m_player_white == GamePlayer::Human) {
                        // muhle_engine::send_message("move " + StandardGameBoard::string_from_move(move) + '\n');
                    }
                    break;
            }
        }
    );
}
