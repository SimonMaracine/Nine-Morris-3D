#include "game/minimax/standard_game/minimax_standard_game.h"

namespace minimax_standard_game {
    int _minimax() {
        return 0;
    }

    void minimax(GamePosition position, MinimaxThread::Result& result, std::atomic<bool>& running) {
        std::this_thread::sleep_for(std::chrono::seconds(3));

        for (size_t i = 0; i < position.size(); i++) {
            if (position[i] == PieceType::None) {
                result.place_node_index = i;
            }
        }

        running.store(false);
    }
}
