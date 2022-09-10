#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/components/board.h"
#include "game/components/undo_redo_state.h"
#include "game/components/camera_player_position.h"

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

    struct SavedGame {
        BoardComponent board_c;
        CameraComponent camera_c;
        unsigned int time = 0;  // In deciseconds
        std::string date = NO_LAST_GAME;
        // StateHistory state_history;
        UndoRedoStateComponent undo_redo_state_c;
        // glm::vec3 white_camera_position = glm::vec3(0.0f);
        // glm::vec3 black_camera_position = glm::vec3(0.0f);
        CameraPlayerPosition camera_player_position_c;
        GamePlayer white_player = GamePlayer::None;
        GamePlayer black_player = GamePlayer::None;
    };

    void save_game_to_file(const SavedGame& saved_game) noexcept(false);
    void load_game_from_file(SavedGame& saved_game) noexcept(false);

    void delete_save_game_file(std::string_view file_path);  // TODO use this, or delete it
    void handle_save_file_not_open_error(std::string_view app_name);
}
