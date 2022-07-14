#include "nine_morris_3d/minimax/main_game/minimax_main_game.h"
#include "nine_morris_3d/board.h"

namespace minimax_main_game {
    int _minimax() {
        return 0;
    }

    void minimax(GamePosition position, MinimaxThread::Result& result, std::atomic<bool>& running) {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        for (size_t i = 0; i < position.size(); i++) {
            if (position[i] == PosValue::None) {
                result.place_node_index = i; 
            }
        }

        running.store(false);
    }
}
