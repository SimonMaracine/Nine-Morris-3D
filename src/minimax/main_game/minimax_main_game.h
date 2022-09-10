#pragma once

#include "minimax/minimax_thread.h"
#include "game/constants.h"

namespace minimax_main_game {
    int _minimax();
    void minimax(GamePosition position, MinimaxThread::Result& result, std::atomic<bool>& running);
}
