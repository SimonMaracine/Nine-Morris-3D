#pragma once

#include <string>

namespace user_data {
    const std::string get_username();
    const std::string get_user_data_path();
    bool user_data_directory_exists();
    bool create_user_data_directory();
}
