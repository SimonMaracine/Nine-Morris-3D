#pragma once

#include <string>
#include <stdexcept>

#include "graphics/renderer/camera.h"
#include "nine_morris_3d/board.h"

namespace save_load {
    class SaveFileError : public std::runtime_error {
    public:
        SaveFileError(const std::string& message)
            : std::runtime_error(message) {}
        SaveFileError(const char* message)
            : std::runtime_error(message) {}
    };

    class SaveFileNotOpenError : public SaveFileError {
    public:
        SaveFileNotOpenError(const std::string& message)
            : SaveFileError(message) {}
        SaveFileNotOpenError(const char* message)
            : SaveFileError(message) {}
    };

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

    void save_game_to_file(const GameState& game_state) noexcept(false);
    void load_game_from_file(GameState& game_state) noexcept(false);

    void delete_save_game_file(const std::string& file_path);  // TODO use this, or delete it
    void handle_save_file_not_open_error();
}
