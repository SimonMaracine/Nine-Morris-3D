#pragma once

#include "nine_morris_3d/board.h"
#include "nine_morris_3d/minimax/minimax_thread.h"

namespace minimax_main_game {
    int _minimax();
    void minimax(GamePosition position, MinimaxThread::Result& result, std::atomic<bool>& running);
}
