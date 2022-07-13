#include "nine_morris_3d/minimax/main_game/minimax_main_game.h"
#include "nine_morris_3d/board.h"

namespace minimax_main_game {
    void place(GamePosition position, size_t* node_index, std::atomic<bool>& running) {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        for (size_t i = 0; i < position.size(); i++) {
            if (position[i] == PosValue::None) {
                *node_index = i; 
            }
        }

        running.store(false);
    }

    void take(GamePosition position, size_t* node_index, std::atomic<bool>& running) {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        *node_index = 2;

        running.store(false);
    }

    void put_down(GamePosition position, size_t* source_node_index, size_t* destination_node_index, std::atomic<bool>& running) {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        *source_node_index = 3;
        *destination_node_index = 2;

        running.store(false);
    }
}
