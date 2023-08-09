#pragma once

#include <string>
#include <string_view>
#include <stdexcept>

namespace sm {
    struct FileSystem {  // TODO use C++ 17 filesystem
        class UserNameError : public std::runtime_error {
        public:
            explicit UserNameError(const std::string& message)
                : std::runtime_error(message) {}
            explicit UserNameError(const char* message)
                : std::runtime_error(message) {}
        };

        // There is no uninitialization
        static void initialize_for_applications(std::string_view application_name) noexcept(false);

        static bool directory_exists(std::string_view path);  // Path must not end with trailing backslash
        static bool create_directory(std::string_view path);
        static bool delete_file(std::string_view path);

        static std::string cut_slash(std::string_view path);

        static std::string get_user_name() noexcept(false);
        static void check_and_fix_directories();

        static std::string path_for_logs();
        static std::string path_for_saved_data();
        static std::string path_for_assets();

        static std::string path_for_logs(std::string_view file);
        static std::string path_for_saved_data(std::string_view file);
        static std::string path_for_assets(std::string_view file_path);
    };
}
