#pragma once

#include <string>

#define APP_NAME_LINUX "ninemorris3d"
#define APP_NAME_WINDOWS "NineMorris3D"

namespace user_data {
    const std::string get_username();
    const std::string get_user_data_directory_path();
    bool user_data_directory_exists();
    bool create_user_data_directory();
}
