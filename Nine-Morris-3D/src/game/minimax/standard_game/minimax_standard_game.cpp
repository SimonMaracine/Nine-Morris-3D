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

constexpr int MINIMUM_EVALUATION_VALUE = INT_MIN;
constexpr int MAXIMUM_EVALUATION_VALUE = INT_MAX;

namespace values {
    static constexpr int PIECE = 10;
    static constexpr int FREEDOM = 1;
    // static constexpr int  = 10;
}

static unsigned int calculate_material(GamePosition position, PieceType type) {
    unsigned int piece_count = 0;

    for (size_t i = 0; i < MAX_NODES; i++) {
        piece_count += position.at(i) == type;
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

static unsigned int calculate_freedom(GamePosition position, PieceType type) {
    unsigned int total_free_positions = 0;

    for (size_t i = 0; i < MAX_NODES; i++) {
        if (position.at(i) == type) {
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

static std::vector<GamePosition> get_all_valid_moves(GamePosition position, PieceType type) {
    std::vector<GamePosition> moves;

    if (calculate_material(position, type) == 3) {  // Phase 3


        return {};
    }

    if (position.turns < 18) {  // Phase 1

    } else {  // Phase 2

    }
}

static int _minimax(GamePosition position, size_t depth, bool maximizing_player) {
    if (depth == 0 || is_game_over(position)) {
        return evaluate_position(position);
    }

    if (maximizing_player) {
        int max_evaluation = MINIMUM_EVALUATION_VALUE;

        for (const GamePosition& new_position : get_all_valid_moves(position, PieceType::White)) {
            const int evaluation = _minimax(new_position, depth - 1, false);
            max_evaluation = std::max(max_evaluation, evaluation);
        }

        return max_evaluation;
    } else {
        int min_evaluation = MAXIMUM_EVALUATION_VALUE;

        for (const GamePosition& new_position : get_all_valid_moves(position, PieceType::Black)) {
            const int evaluation = _minimax(new_position, depth - 1, true);
            min_evaluation = std::min(min_evaluation, evaluation);
        }

        return min_evaluation;
    }
}

namespace minimax_standard_game {
    void minimax(GamePosition position, MinimaxThread::Result& result, std::atomic<bool>& running) {
        const auto what = _minimax(position, 4, true);

        result.place_node_index = 917437;

        running.store(false);
    }
}
