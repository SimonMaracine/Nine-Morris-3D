#include <nine_morris_3d_engine/engine_other.h>

#include "game/game_context.h"
#include "game/entities/board.h"
#include "game/minimax/minimax_thread.h"
#include "game/minimax/standard_game/minimax_standard_game.h"
#include "other/constants.h"

void GameContext::begin_human_move() {
    board->is_players_turn = true;
}

void GameContext::end_human_move() {
    // Nothing needs to be done for now
}

void GameContext::begin_computer_move() {
    board->is_players_turn = false;

    // TODO start the appropriate minimax algorithm in thread

    if (board->must_take_piece) {
        return;  // Do nothing in this case
    }

    minimax_thread->start(minimax_standard_game::minimax);
}

bool GameContext::end_computer_move() {
    switch (board->phase) {
        case BoardPhase::PlacePieces:
            if (board->must_take_piece) {
                const auto& result = minimax_thread->get_result();
                board->computer_take_piece(result.take_node_index);
            } else {
                const auto& result = minimax_thread->get_result();
                board->computer_place_piece(result.place_node_index);
            }
            break;
        case BoardPhase::MovePieces:
            if (board->must_take_piece) {
                const auto& result = minimax_thread->get_result();
                board->computer_take_piece(result.take_node_index);
            } else {
                const auto& result = minimax_thread->get_result();
                board->computer_move_piece(
                    result.put_down_source_node_index,
                    result.put_down_destination_node_index
                );
            }
            break;
        default:
            break;
    }

    const bool result = board->switched_turn;
    board->switched_turn = false;  // These need to be reset
    board->must_take_piece_or_took_piece = false;

    return result;
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
