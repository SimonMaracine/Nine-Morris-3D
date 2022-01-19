#pragma once

#include <vector>
#include <memory>

#include "graphics/renderer/camera.h"
#include "nine_morris_3d/board.h"

namespace save_load {
    struct GameState {
        Board board;
        Camera camera;
        unsigned int time;  // In deciseconds

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(board, camera, time);
        }
    };

    void save_game(const GameState& game_state);
    void load_game(GameState& game_state);

    bool save_files_exist();
}
