#pragma once

#include <string>
#include <stdexcept>

#define APP_NAME_LINUX "ninemorris3d"
#define APP_NAME_WINDOWS "NineMorris3D"

/**
 * These are functions for getting platform specific username and user directories.
 */
namespace user_data {
    class UserNameError : public std::runtime_error {
    public:
        UserNameError(const std::string& message)
            : std::runtime_error(message) {}
        UserNameError(const char* message)
            : std::runtime_error(message) {}
    };

    const std::string get_username();
    const std::string get_user_data_directory_path();
    bool user_data_directory_exists();
    bool create_user_data_directory();
}
