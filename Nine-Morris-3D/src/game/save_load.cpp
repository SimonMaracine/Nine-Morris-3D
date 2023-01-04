#include <engine/engine_other.h>

#include "game/save_load.h"

static constexpr const char* PARTIAL_SAVE_GAME_FILE = "_last_game.dat";

namespace save_load {
    void handle_save_file_not_open_error() {
        const bool exists = file_system::directory_exists(
            file_system::cut_slash(file_system::path_for_saved_data())
        );

        if (!exists) {
            REL_INFO("User data directory missing; creating it...");

            const bool success = file_system::create_directory(file_system::path_for_saved_data());

            if (!success) {
                REL_ERROR("Could not create user data directory");
                return;
            }
        }
    }

    std::string save_game_file_name(std::string_view name) {
        return std::string(name) + PARTIAL_SAVE_GAME_FILE;
    }
}
