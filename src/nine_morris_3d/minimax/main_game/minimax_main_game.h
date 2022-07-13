#pragma once

#include "nine_morris_3d/board.h"

namespace minimax_main_game {
    void place(GamePosition position, size_t* node_index, std::atomic<bool>& running);
    void take(GamePosition position, size_t* node_index, std::atomic<bool>& running);
    void put_down(GamePosition position, size_t* source_node_index, size_t* destination_node_index, std::atomic<bool>& running);
}
