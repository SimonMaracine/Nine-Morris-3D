#include "nine_morris_3d/game_context.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/minimax/minimax_thread.h"
#include "nine_morris_3d/minimax/main_game/minimax_main_game.h"

void GameContext::begin_human_move() {
    board->is_players_turn = true;
}

void GameContext::end_human_move() {
    board->is_players_turn = false;
}

void GameContext::begin_computer_move() {
    board->is_players_turn = false;

    // TODO start the appropriate minimax algorithm in thread

    if (board->phase == Board::Phase::PlacePieces) {
        if (board->should_take_piece) {
            minimax_thread->start_take(minimax_main_game::take);
        } else {
            minimax_thread->start_place(minimax_main_game::place);
        }
    } else if (board->phase == Board::Phase::MovePieces) {
        if (board->should_take_piece) {
            minimax_thread->start_take(minimax_main_game::take);
        } else {
            minimax_thread->start_put_down(minimax_main_game::put_down);
        }
    }
}

void GameContext::end_computer_move() {
    board->is_players_turn = true;

    if (board->phase == Board::Phase::PlacePieces) {
        if (board->should_take_piece) {
            board->computer_take_piece(minimax_thread->get_take_result().node_index);
        } else {
            board->computer_place_piece(minimax_thread->get_place_result().node_index);
        }
    } else if (board->phase == Board::Phase::MovePieces) {
        if (board->should_take_piece) {
            board->computer_take_piece(minimax_thread->get_take_result().node_index);
        } else {
            const MinimaxThread::ResultPutDown& result = minimax_thread->get_put_down_result();
            board->computer_put_down_piece(result.source_node_index, result.destination_node_index);
        }
    }
}
