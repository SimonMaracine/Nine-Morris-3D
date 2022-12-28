#pragma once

namespace file_system {
    class UserNameError : public std::runtime_error {
    public:
        explicit UserNameError(const std::string& message)
            : std::runtime_error(message) {}
        explicit UserNameError(const char* message)
            : std::runtime_error(message) {}
    };

    void initialize_for_applications(std::string_view application_name) noexcept(false);

    bool directory_exists(std::string_view path);
    bool create_directory(std::string_view path);
    bool delete_file(std::string_view path);

    std::string get_user_name() noexcept(false);
    void check_and_fix_directories();

    std::string path_for_logs();
    std::string path_for_saved_data();
    std::string path_for_assets();

    std::string path_for_logs(std::string_view file);
    std::string path_for_saved_data(std::string_view file);
    std::string path_for_assets(std::string_view file_path);
}
