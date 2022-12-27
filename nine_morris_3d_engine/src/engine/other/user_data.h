#pragma once

namespace user_data {
    class UserNameError : public std::runtime_error {
    public:
        UserNameError(const std::string& message)
            : std::runtime_error(message) {}
        UserNameError(const char* message)
            : std::runtime_error(message) {}
    };

    bool directory_exists(std::string_view path);
    bool create_directory(std::string_view path);

    std::string get_username() noexcept(false);
    std::string get_user_data_directory_path(std::string_view application_name) noexcept(false);
    bool user_data_directory_exists(std::string_view application_name) noexcept(false);
    bool create_user_data_directory(std::string_view application_name) noexcept(false);
}
