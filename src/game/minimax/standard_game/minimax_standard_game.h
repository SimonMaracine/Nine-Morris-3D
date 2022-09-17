#pragma once

#include "game/minimax/minimax_thread.h"
#include "game/constants.h"

namespace minimax_standard_game {
    int _minimax();
    void minimax(GamePosition position, MinimaxThread::Result& result, std::atomic<bool>& running);
}
