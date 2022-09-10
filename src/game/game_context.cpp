#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/game_context.h"
#include "game/board/board.h"
#include "minimax/minimax_thread.h"
#include "minimax/main_game/minimax_main_game.h"

void GameContext::begin_human_move() {
    board->is_players_turn = true;
}

void GameContext::end_human_move() {
    board->is_players_turn = false;
}

void GameContext::begin_computer_move() {
    // TODO start the appropriate minimax algorithm in thread

    if (board->should_take_piece) {
        return;  // Do nothing in this case
    }

    minimax_thread->start(minimax_main_game::minimax);
}

void GameContext::end_computer_move() {
    if (board->phase == Board::Phase::PlacePieces) {
        if (board->should_take_piece) {
            board->computer_take_piece(minimax_thread->get_result().take_node_index);
        } else {
            board->computer_place_piece(minimax_thread->get_result().place_node_index);
        }
    } else if (board->phase == Board::Phase::MovePieces) {
        if (board->should_take_piece) {
            board->computer_take_piece(minimax_thread->get_result().take_node_index);
        } else {
            const MinimaxThread::Result& result = minimax_thread->get_result();
            board->computer_put_down_piece(result.put_down_source_node_index, result.put_down_destination_node_index);
        }
    }
}

void GameContext::reset_player(GamePlayer player) {
    switch (player) {
        case GamePlayer::None:
            ASSERT(false, "Player must not be None");
            break;
        case GamePlayer::Human:
            board->is_players_turn = false;
            break;
        case GamePlayer::Computer:
            break;  // Nothing needs to be done
    }

    state = GameState::MaybeNextPlayer;
}
