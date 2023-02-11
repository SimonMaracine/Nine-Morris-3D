#pragma once

#include "game/minimax/minimax_thread.h"
#include "game/game_position.h"
#include "other/constants.h"

namespace minimax_standard_game {
    void minimax(GamePosition position, MinimaxThread::Result& result, std::atomic<bool>& running);
}
