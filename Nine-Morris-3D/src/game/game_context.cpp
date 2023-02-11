#include <engine/engine_other.h>

#include "game/game_context.h"
#include "game/entities/board.h"
#include "game/minimax/minimax_thread.h"
#include "game/minimax/standard_game/minimax_standard_game.h"
#include "other/constants.h"

void GameContext::human_begin_move() {
    board->is_players_turn = true;
}

void GameContext::computer_think_move() {
    board->is_players_turn = false;

    // TODO start the appropriate minimax algorithm in thread

    minimax_thread->start(minimax_standard_game::minimax);
}

bool GameContext::computer_execute_move() {
    switch (board->phase) {
        case BoardPhase::PlacePieces: {
            const auto& result = minimax_thread->get_result();
            board->place_piece(result.place_node_index);

            break;
        }
        case BoardPhase::MovePieces: {
            const auto& result = minimax_thread->get_result();
            board->move_piece(
                result.put_down_source_node_index,
                result.put_down_destination_node_index
            );

            break;
        }
        default:
            break;
    }

    const bool result_switched = board->flags.switched_turn;
    board->flags.switched_turn = false;  // These need to be reset
    board->flags.must_take_or_took_piece = false;

    return result_switched;
}

void GameContext::computer_execute_take_move() {
    ASSERT(board->must_take_piece, "Everything is wrong");

    const auto& result = minimax_thread->get_result();
    board->take_piece(result.take_node_index);

    const bool result_switched = board->flags.switched_turn;
    board->flags.switched_turn = false;  // These need to be reset
    board->flags.must_take_or_took_piece = false;

    ASSERT(result_switched, "Turn must have switched");
}

void GameContext::reset_players() {
    board->is_players_turn = false;
    state = GameState::NextPlayer;
}
