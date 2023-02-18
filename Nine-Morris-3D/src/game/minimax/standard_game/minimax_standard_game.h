#pragma once

#include "game/minimax/common.h"
#include "game/game_position.h"
#include "other/constants.h"

namespace minimax_standard_game {
    void minimax(GamePosition position, PieceType piece, Move& result, std::atomic<bool>& running);
}
