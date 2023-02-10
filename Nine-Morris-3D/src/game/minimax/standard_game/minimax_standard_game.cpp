#include "game/minimax/standard_game/minimax_standard_game.h"
#include "other/constants.h"

/*
    Maximizing player is white.
    Minimizing player is black.
*/

static bool is_game_over(GamePosition position) {
    return {};
}

static int static_evaluation(GamePosition position) {
    return {};
}

static std::vector<GamePosition> get_valid_moves(GamePosition position, bool maximizing_player) {
    return {};
}

static int _minimax(GamePosition position, size_t depth, bool maximizing_player) {
    if (depth == 0 || is_game_over(position)) {
        return static_evaluation(position);
    }

    if (maximizing_player) {
        int max_evaluation = INT_MIN;

        for (GamePosition new_position : get_valid_moves(position, maximizing_player)) {
            const int evaluation = _minimax(new_position, depth - 1, false);
            max_evaluation = std::max(max_evaluation, evaluation);
        }

        return max_evaluation;
    } else {
        int min_evaluation = INT_MAX;

        for (GamePosition new_position : get_valid_moves(position, maximizing_player)) {
            const int evaluation = _minimax(new_position, depth - 1, true);
            min_evaluation = std::min(min_evaluation, evaluation);
        }

        return min_evaluation;
    }
}

namespace minimax_standard_game {
    void minimax(GamePosition position, MinimaxThread::Result& result, std::atomic<bool>& running) {
        const auto what = _minimax(position, 3, true);

        result.place_node_index = 917437;

        running.store(false);
    }
}
