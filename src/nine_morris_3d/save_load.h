#pragma once

#include <vector>

#include "opengl/renderer/camera.h"
#include "nine_morris_3d/board.h"

namespace save_load {
    struct GameState {
        Board board;
        Camera camera;
        std::vector<Board> board_state_history;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(board, camera, board_state_history);
        }
    };

    void save_game(const GameState& game_state);
    void load_game(GameState& game_state);

    bool save_files_exist();
}
