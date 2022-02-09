#pragma once

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

    void save_game_to_file(const GameState& game_state);
    void load_game_from_file(GameState& game_state);

    void delete_save_game_file(const std::string& file_path);
    bool save_files_exist();
}
