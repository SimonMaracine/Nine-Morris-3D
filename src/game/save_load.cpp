#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/save_load.h"

namespace save_load {
    void delete_save_game_file(std::string_view file_path) {  // TODO use this, or delete it
        if (remove(file_path.data()) != 0) {
            REL_INFO("Could not delete save game file `{}`", file_path);
        } else {
            REL_INFO("Deleted save game file `{}`", file_path);
        }
    }

    void handle_save_file_not_open_error(std::string_view app_name) {
        bool user_data_directory;

        try {
            user_data_directory = user_data::user_data_directory_exists(app_name);
        } catch (const user_data::UserNameError& e) {
            REL_ERROR("{}", e.what());
            return;
        }

        if (!user_data_directory) {
            REL_INFO("User data folder missing; creating one...");

            try {
                const bool success = user_data::create_user_data_directory(app_name);
                if (!success) {
                    REL_ERROR("Could not create user data directory");
                    return;
                }
            } catch (const user_data::UserNameError& e) {  // TODO message can be better
                REL_ERROR("{}", e.what());
                return;
            }
        }
    }
}
