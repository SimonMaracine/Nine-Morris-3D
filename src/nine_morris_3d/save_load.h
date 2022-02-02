#pragma once

#include <vector>
#include <memory>
#include <string>

#include "graphics/renderer/camera.h"
#include "nine_morris_3d/board.h"

namespace save_load {
    struct GameState {
        Board board;
        Camera camera;
        unsigned int time = 0;  // In deciseconds
        std::string date = "No Last Date";

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(board, camera, time, date);
        }
    };

    void save_game(const GameState& game_state);
    void load_game(GameState& game_state);

    bool save_files_exist();
}
