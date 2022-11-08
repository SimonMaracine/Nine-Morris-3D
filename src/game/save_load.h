#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/undo_redo_state.h"
#include "other/constants.h"

namespace save_load {
    static const char* SAVE_GAME_FILE = "last_game.dat";
    static const char* NO_LAST_GAME = "No Last Game";

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

    static std::string get_file_path() noexcept(false) {
        std::string file_path;

        try {
            file_path = paths::path_for_saved_data(SAVE_GAME_FILE);
        } catch (const user_data::UserNameError& e) {
            throw SaveFileError(e.what());
        }

        return file_path;
    }

    template<typename B>
    struct SavedGame {
        B board;
        Camera camera;
        unsigned int time = 0;  // In deciseconds
        std::string date = NO_LAST_GAME;
        UndoRedoState<B> undo_redo_state;
        GamePlayer white_player = GamePlayer::None;
        GamePlayer black_player = GamePlayer::None;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(
                // board,
                // camera,
                time,
                date,
                // undo_redo_state,
                white_player,
                black_player
            );
        }
    };

    template<typename B>
    void save_game_to_file(const SavedGame<B>& saved_game) noexcept(false) {
        const std::string file_path = get_file_path();

        std::ofstream file {file_path, std::ios::binary | std::ios::trunc};

        if (!file.is_open()) {
            throw SaveFileNotOpenError(
                "Could not open last save game file `" + std::string(SAVE_GAME_FILE) + "` for writing"
            );
        }

        try {
            cereal::BinaryOutputArchive output {file};
            output(saved_game);
        } catch (const std::exception& e) {  // Just to be sure...
            throw SaveFileError(e.what());
        }

        DEB_INFO("Saved game to file `{}`", SAVE_GAME_FILE);
    }

    template<typename B>
    void load_game_from_file(SavedGame<B>& saved_game) noexcept(false) {
        const std::string file_path = get_file_path();

        std::ifstream file {file_path, std::ios::binary};

        if (!file.is_open()) {
            throw SaveFileNotOpenError(
                "Could not open last save game file `" + std::string(SAVE_GAME_FILE) + "`"
            );
        }

        try {
            cereal::BinaryInputArchive input {file};
            input(saved_game);
        } catch (const cereal::Exception& e) {
            throw SaveFileError(e.what());
        } catch (const std::exception& e) {
            throw SaveFileError(e.what());
        }

        DEB_INFO("Loaded game from file `{}`", SAVE_GAME_FILE);
    }

    void delete_save_game_file(std::string_view file_path);  // TODO use this, or delete it
    void handle_save_file_not_open_error(std::string_view app_name);
}
