#include <engine/other/assert.h>

#include "game/minimax/standard_game/minimax_standard_game.h"
#include "game/game_position.h"
#include "other/constants.h"

// TODO please don't optimize early!!!

/*
    Maximizing player is white.
    Minimizing player is black.

    Evaluation is a signed integer.
    Value of 0 means equal position, positive means white is better, negative means black is better.
*/

static constexpr int MINIMUM_EVALUATION_VALUE = INT_MIN;
static constexpr int MAXIMUM_EVALUATION_VALUE = INT_MAX;

namespace values {
    static constexpr int PIECE = 10;
    static constexpr int FREEDOM = 1;
    // static constexpr int  = 10;
}

static unsigned int calculate_material(GamePosition position, PieceType piece) {
    ASSERT(piece != PieceType::None, "Invalid enum");

    unsigned int piece_count = 0;

    for (size_t i = 0; i < MAX_NODES; i++) {
        piece_count += position.at(i) == piece;
    }

    return piece_count;
}

static unsigned int calculate_piece_freedom(GamePosition position, size_t index) {
    int freedom = 0;

    switch (index) {
        case 0:
            freedom += position.at(1) == PieceType::None;
            freedom += position.at(9) == PieceType::None;
            break;
        case 1:
            freedom += position.at(0) == PieceType::None;
            freedom += position.at(2) == PieceType::None;
            freedom += position.at(4) == PieceType::None;
            break;
        case 2:
            freedom += position.at(1) == PieceType::None;
            freedom += position.at(14) == PieceType::None;
            break;
        case 3:
            freedom += position.at(4) == PieceType::None;
            freedom += position.at(10) == PieceType::None;
            break;
        case 4:
            freedom += position.at(1) == PieceType::None;
            freedom += position.at(3) == PieceType::None;
            freedom += position.at(5) == PieceType::None;
            freedom += position.at(7) == PieceType::None;
            break;
        case 5:
            freedom += position.at(4) == PieceType::None;
            freedom += position.at(13) == PieceType::None;
            break;
        case 6:
            freedom += position.at(7) == PieceType::None;
            freedom += position.at(11) == PieceType::None;
            break;
        case 7:
            freedom += position.at(4) == PieceType::None;
            freedom += position.at(6) == PieceType::None;
            freedom += position.at(8) == PieceType::None;
            break;
        case 8:
            freedom += position.at(7) == PieceType::None;
            freedom += position.at(12) == PieceType::None;
            break;
        case 9:
            freedom += position.at(0) == PieceType::None;
            freedom += position.at(10) == PieceType::None;
            freedom += position.at(21) == PieceType::None;
            break;
        case 10:
            freedom += position.at(3) == PieceType::None;
            freedom += position.at(9) == PieceType::None;
            freedom += position.at(11) == PieceType::None;
            freedom += position.at(18) == PieceType::None;
            break;
        case 11:
            freedom += position.at(6) == PieceType::None;
            freedom += position.at(10) == PieceType::None;
            freedom += position.at(15) == PieceType::None;
            break;
        case 12:
            freedom += position.at(8) == PieceType::None;
            freedom += position.at(13) == PieceType::None;
            freedom += position.at(17) == PieceType::None;
            break;
        case 13:
            freedom += position.at(5) == PieceType::None;
            freedom += position.at(12) == PieceType::None;
            freedom += position.at(14) == PieceType::None;
            freedom += position.at(20) == PieceType::None;
            break;
        case 14:
            freedom += position.at(2) == PieceType::None;
            freedom += position.at(13) == PieceType::None;
            freedom += position.at(23) == PieceType::None;
            break;
        case 15:
            freedom += position.at(11) == PieceType::None;
            freedom += position.at(16) == PieceType::None;
            break;
        case 16:
            freedom += position.at(15) == PieceType::None;
            freedom += position.at(17) == PieceType::None;
            freedom += position.at(19) == PieceType::None;
            break;
        case 17:
            freedom += position.at(12) == PieceType::None;
            freedom += position.at(16) == PieceType::None;
            break;
        case 18:
            freedom += position.at(10) == PieceType::None;
            freedom += position.at(19) == PieceType::None;
            break;
        case 19:
            freedom += position.at(16) == PieceType::None;
            freedom += position.at(18) == PieceType::None;
            freedom += position.at(20) == PieceType::None;
            freedom += position.at(22) == PieceType::None;
            break;
        case 20:
            freedom += position.at(13) == PieceType::None;
            freedom += position.at(19) == PieceType::None;
            break;
        case 21:
            freedom += position.at(9) == PieceType::None;
            freedom += position.at(22) == PieceType::None;
            break;
        case 22:
            freedom += position.at(19) == PieceType::None;
            freedom += position.at(21) == PieceType::None;
            freedom += position.at(23) == PieceType::None;
            break;
        case 23:
            freedom += position.at(14) == PieceType::None;
            freedom += position.at(22) == PieceType::None;
            break;
    }

    return freedom;
}

static unsigned int calculate_freedom(GamePosition position, PieceType piece) {
    ASSERT(piece != PieceType::None, "Invalid enum");

    unsigned int total_free_positions = 0;

    for (size_t i = 0; i < MAX_NODES; i++) {
        if (position.at(i) == piece) {
            total_free_positions += calculate_piece_freedom(position, i);
        }
    }

    return total_free_positions;
}

static bool is_game_over(GamePosition position) {
    return {};
}

static int evaluate_position(GamePosition position) {  // TODO also evaluate positions
    int evaluation = 0;

    const unsigned int white_material = calculate_material(position, PieceType::White);
    const unsigned int black_material = calculate_material(position, PieceType::Black);

    evaluation += white_material * values::PIECE;
    evaluation -= black_material * values::PIECE;

    const unsigned int white_freedom = calculate_freedom(position, PieceType::White);
    const unsigned int black_freedom = calculate_freedom(position, PieceType::Black);

    evaluation += white_freedom * values::FREEDOM;
    evaluation -= black_freedom * values::FREEDOM;

    return evaluation;
}

#define IS_PC(const_index) (position.at(const_index) == piece)

static bool is_mill_made(GamePosition position, PieceType piece, size_t index) {
    ASSERT(piece != PieceType::None, "Invalid enum");

    switch (index) {
        case 0:
            if (IS_PC(1) && IS_PC(2) || IS_PC(9) && IS_PC(21))
                return true;
            break;
        case 1:
            if (IS_PC(0) && IS_PC(2) || IS_PC(4) && IS_PC(7))
                return true;
            break;
        case 2:
            if (IS_PC(0) && IS_PC(1) || IS_PC(14) && IS_PC(23))
                return true;
            break;
        case 3:
            if (IS_PC(4) && IS_PC(5) || IS_PC(10) && IS_PC(18))
                return true;
            break;
        case 4:
            if (IS_PC(3) && IS_PC(5) || IS_PC(1) && IS_PC(7))
                return true;
            break;
        case 5:
            if (IS_PC(3) && IS_PC(4) || IS_PC(13) && IS_PC(20))
                return true;
            break;
        case 6:
            if (IS_PC(7) && IS_PC(8) || IS_PC(11) && IS_PC(15))
                return true;
            break;
        case 7:
            if (IS_PC(6) && IS_PC(8) || IS_PC(1) && IS_PC(4))
                return true;
            break;
        case 8:
            if (IS_PC(6) && IS_PC(7) || IS_PC(12) && IS_PC(17))
                return true;
            break;
        case 9:
            if (IS_PC(0) && IS_PC(21) || IS_PC(10) && IS_PC(11))
                return true;
            break;
        case 10:
            if (IS_PC(9) && IS_PC(11) || IS_PC(3) && IS_PC(18))
                return true;
            break;
        case 11:
            if (IS_PC(9) && IS_PC(10) || IS_PC(6) && IS_PC(15))
                return true;
            break;
        case 12:
            if (IS_PC(13) && IS_PC(14) || IS_PC(8) && IS_PC(17))
                return true;
            break;
        case 13:
            if (IS_PC(12) && IS_PC(14) || IS_PC(5) && IS_PC(20))
                return true;
            break;
        case 14:
            if (IS_PC(12) && IS_PC(13) || IS_PC(2) && IS_PC(23))
                return true;
            break;
        case 15:
            if (IS_PC(16) && IS_PC(17) || IS_PC(6) && IS_PC(11))
                return true;
            break;
        case 16:
            if (IS_PC(15) && IS_PC(17) || IS_PC(19) && IS_PC(22))
                return true;
            break;
        case 17:
            if (IS_PC(15) && IS_PC(16) || IS_PC(8) && IS_PC(12))
                return true;
            break;
        case 18:
            if (IS_PC(19) && IS_PC(20) || IS_PC(3) && IS_PC(10))
                return true;
            break;
        case 19:
            if (IS_PC(18) && IS_PC(20) || IS_PC(16) && IS_PC(22))
                return true;
            break;
        case 20:
            if (IS_PC(18) && IS_PC(19) || IS_PC(5) && IS_PC(13))
                return true;
            break;
        case 21:
            if (IS_PC(22) && IS_PC(23) || IS_PC(0) && IS_PC(9))
                return true;
            break;
        case 22:
            if (IS_PC(21) && IS_PC(23) || IS_PC(16) && IS_PC(19))
                return true;
            break;
        case 23:
            if (IS_PC(21) && IS_PC(22) || IS_PC(2) && IS_PC(14))
                return true;
            break;
    }

    return false;
}

static constexpr PieceType opponent_piece(PieceType type) {
    ASSERT(type != PieceType::None, "Invalid enum");

    if (type == PieceType::White) {
        return PieceType::Black;
    } else {
        return PieceType::White;
    }
}

#define IS_FREE_CHECK(const_index) \
    if (position.at(const_index) == PieceType::None) { \
        result[pos++] = (const_index); \
    }

std::array<size_t, 4> neighbor_free_positions(GamePosition position, PieceType piece, size_t index) {
    ASSERT(piece != PieceType::None, "Invalid enum");

    std::array<size_t, 4> result = { NULL_INDEX, NULL_INDEX, NULL_INDEX, NULL_INDEX };
    size_t pos = 0;

    switch (index) {
        case 0:
            IS_FREE_CHECK(1)
            IS_FREE_CHECK(9)
            break;
        case 1:
            IS_FREE_CHECK(0)
            IS_FREE_CHECK(2)
            IS_FREE_CHECK(4)
            break;
        case 2:
            IS_FREE_CHECK(1)
            IS_FREE_CHECK(14)
            break;
        case 3:
            IS_FREE_CHECK(4)
            IS_FREE_CHECK(10)
            IS_FREE_CHECK(4)
            break;
        case 4:
            IS_FREE_CHECK(1)
            IS_FREE_CHECK(3)
            IS_FREE_CHECK(5)
            IS_FREE_CHECK(7)
            break;
        case 5:
            IS_FREE_CHECK(4)
            IS_FREE_CHECK(13)
            break;
        case 6:
            IS_FREE_CHECK(7)
            IS_FREE_CHECK(11)
            break;
        case 7:
            IS_FREE_CHECK(4)
            IS_FREE_CHECK(6)
            IS_FREE_CHECK(8)
            break;
        case 8:
            IS_FREE_CHECK(7)
            IS_FREE_CHECK(12)
            break;
        case 9:
            IS_FREE_CHECK(0)
            IS_FREE_CHECK(10)
            IS_FREE_CHECK(21)
            break;
        case 10:
            IS_FREE_CHECK(3)
            IS_FREE_CHECK(9)
            IS_FREE_CHECK(11)
            IS_FREE_CHECK(18)
            break;
        case 11:
            IS_FREE_CHECK(6)
            IS_FREE_CHECK(10)
            IS_FREE_CHECK(15)
            break;
        case 12:
            IS_FREE_CHECK(8)
            IS_FREE_CHECK(13)
            IS_FREE_CHECK(17)
            break;
        case 13:
            IS_FREE_CHECK(5)
            IS_FREE_CHECK(12)
            IS_FREE_CHECK(14)
            IS_FREE_CHECK(20)
            break;
        case 14:
            IS_FREE_CHECK(2)
            IS_FREE_CHECK(13)
            IS_FREE_CHECK(23)
            break;
        case 15:
            IS_FREE_CHECK(11)
            IS_FREE_CHECK(16)
            break;
        case 16:
            IS_FREE_CHECK(15)
            IS_FREE_CHECK(17)
            IS_FREE_CHECK(19)
            break;
        case 17:
            IS_FREE_CHECK(12)
            IS_FREE_CHECK(16)
            break;
        case 18:
            IS_FREE_CHECK(10)
            IS_FREE_CHECK(19)
            break;
        case 19:
            IS_FREE_CHECK(16)
            IS_FREE_CHECK(18)
            IS_FREE_CHECK(20)
            IS_FREE_CHECK(22)
            break;
        case 20:
            IS_FREE_CHECK(13)
            IS_FREE_CHECK(19)
            break;
        case 21:
            IS_FREE_CHECK(9)
            IS_FREE_CHECK(22)
            break;
        case 22:
            IS_FREE_CHECK(19)
            IS_FREE_CHECK(21)
            IS_FREE_CHECK(23)
            break;
        case 23:
            IS_FREE_CHECK(14)
            IS_FREE_CHECK(22)
            break;
    }

    return result;
}

enum class MoveType {
    None,
    Place,
    Move,
    PlaceTake,
    MoveTake
};

struct Move {  // TODO can use union
    size_t place_node_index = NULL_INDEX;
    size_t take_node_index = NULL_INDEX;
    size_t put_down_source_node_index = NULL_INDEX;
    size_t put_down_destination_node_index = NULL_INDEX;

    MoveType type = MoveType::None;

    static Move createPlace(size_t place_node_index) {
        Move move;
        move.type = MoveType::Place;
        move.place_node_index = place_node_index;
        return move;
    }

    static Move createMove(size_t put_down_source_node_index, size_t put_down_destination_node_index) {
        Move move;
        move.type = MoveType::Move;
        move.put_down_source_node_index = put_down_source_node_index;
        move.put_down_destination_node_index = put_down_destination_node_index;
        return move;
    }

    static Move createPlaceTake(size_t place_node_index, size_t take_node_index) {
        Move move;
        move.type = MoveType::PlaceTake;
        move.place_node_index = place_node_index;
        move.take_node_index = take_node_index;
        return move;
    }

    static Move createMoveTake(size_t put_down_source_node_index, size_t put_down_destination_node_index, size_t take_node_index) {
        Move move;
        move.type = MoveType::MoveTake;
        move.put_down_source_node_index = put_down_source_node_index;
        move.put_down_destination_node_index = put_down_destination_node_index;
        move.take_node_index = take_node_index;
        return move;
    }
};

static void get_moves_phase1(GamePosition position, PieceType piece, std::vector<Move>& moves) {
    ASSERT(piece != PieceType::None, "Invalid enum");

    for (size_t i = 0; i < MAX_NODES; i++) {
        if (position.at(i) == PieceType::None) {
            if (is_mill_made(position, piece, i)) {
                for (size_t j = 0; j < MAX_NODES; j++) {
                    if (position.at(j) == opponent_piece(piece)) {
                        moves.push_back(Move::createPlaceTake(i, j));
                    }
                }
            } else {
                moves.push_back(Move::createPlace(i));
            }
        }
    }
}

static void get_moves_phase2(GamePosition position, PieceType piece, std::vector<Move>& moves) {
    ASSERT(piece != PieceType::None, "Invalid enum");

    for (size_t i = 0; i < MAX_NODES; i++) {
        if (position.at(i) == piece) {
            const auto free_positions = neighbor_free_positions(position, piece, i);

            for (size_t j = 0; free_positions[j] != NULL_INDEX; j++) {
                if (is_mill_made(position, piece, i)) {
                    for (size_t k = 0; k < MAX_NODES; k++) {
                        if (position.at(k) == opponent_piece(piece)) {
                            moves.push_back(Move::createMoveTake(i, j, k));
                        }
                    }
                } else {
                    moves.push_back(Move::createMove(i, j));
                }
            }
        }
    }
}

static void get_moves_phase3(GamePosition position, PieceType piece, std::vector<Move>& moves) {
    ASSERT(piece != PieceType::None, "Invalid enum");
}

static Move best_move;  // TODO refactor into a class

static std::vector<Move> get_all_moves(GamePosition position, PieceType piece) {
    ASSERT(piece != PieceType::None, "Invalid enum");

    std::vector<Move> moves;

    if (calculate_material(position, piece) == 3) {  // Phase 3
        get_moves_phase3(position, piece, moves);
        return moves;
    }

    if (position.turns < 18) {  // Phase 1
        get_moves_phase1(position, piece, moves);
    } else {  // Phase 2
        get_moves_phase2(position, piece, moves);
    }

    return moves;
}

static void make_move(GamePosition position, const Move& move) {

}

static void unmake_move(GamePosition position, const Move& move) {

}

static int _minimax(GamePosition position, size_t depth, size_t turns_from_root, PieceType type) {  // TODO merge those two parts
    ASSERT(type != PieceType::None, "Invalid enum");

    if (depth == 0 || is_game_over(position)) {
        return evaluate_position(position);
    }

    if (type == PieceType::White) {
        int max_evaluation = MINIMUM_EVALUATION_VALUE;

        const auto moves = get_all_moves(position, PieceType::White);

        for (const Move& move : moves) {
            make_move(position, move);
            const int evaluation = _minimax(position, depth - 1, turns_from_root + 1, PieceType::Black);
            unmake_move(position, move);

            if (evaluation > max_evaluation) {
                max_evaluation = evaluation;

                if (turns_from_root == 0) {
                    best_move = move;
                }
            }
        }

        return max_evaluation;
    } else {
        int min_evaluation = MAXIMUM_EVALUATION_VALUE;

        const auto moves = get_all_moves(position, PieceType::Black);

        for (const Move& move : moves) {
            make_move(position, move);
            const int evaluation = _minimax(position, depth - 1, turns_from_root + 1, PieceType::White);
            unmake_move(position, move);

            if (evaluation < min_evaluation) {
                min_evaluation = evaluation;

                if (turns_from_root == 0) {
                    best_move = move;
                }
            }
        }

        return min_evaluation;
    }
}

namespace minimax_standard_game {
    void minimax(GamePosition position, MinimaxThread::Result& result, std::atomic<bool>& running) {
        // Initialize variables
        best_move = Move {};

        const auto what = _minimax(position, 4, 0, PieceType::White);

        result.place_node_index = 917437;

        running.store(false);
    }
}
