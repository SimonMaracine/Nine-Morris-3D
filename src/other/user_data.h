#pragma once

namespace user_data {
    class UserNameError : public std::runtime_error {
    public:
        UserNameError(const std::string& message)
            : std::runtime_error(message) {}
        UserNameError(const char* message)
            : std::runtime_error(message) {}
    };

    std::string get_username() noexcept(false);
    std::string get_user_data_directory_path(const char* app_name) noexcept(false);
    bool user_data_directory_exists(const char* app_name) noexcept(false);
    bool create_user_data_directory(const char* app_name) noexcept(false);
}
