#pragma once

#include "graphics/renderer/camera.h"
#include "nine_morris_3d/board.h"
#include "nine_morris_3d/undo_redo.h"

namespace save_load {
    static constexpr const char* SAVE_GAME_FILE = "last_game.dat";
    static constexpr const char* NO_LAST_GAME = "No Last Game";

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
        std::string date = NO_LAST_GAME;
        StateHistory state_history;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(board, camera, time, date, state_history);
        }
    };

    void save_game_to_file(const GameState& game_state) noexcept(false);
    void load_game_from_file(GameState& game_state) noexcept(false);

    void delete_save_game_file(std::string_view file_path);  // TODO use this, or delete it
    void handle_save_file_not_open_error();
}
