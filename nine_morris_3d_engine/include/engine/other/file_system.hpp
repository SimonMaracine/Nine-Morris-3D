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
        static void initialize_applications(std::string_view app_name, std::string_view res_directory) noexcept(false);

        static bool directory_exists(std::string_view path);  // Path must not end with trailing backslash
        static bool create_directory(std::string_view path);
        static bool delete_file(std::string_view path);

        static std::string cut_slash(std::string_view path);

        static std::string get_user_name() noexcept(false);
        static void check_and_fix_directories();

        static std::string path_logs();
        static std::string path_saved_data();
        static std::string path_assets();
        static std::string path_engine_data();

        static std::string path_logs(std::string_view file);
        static std::string path_saved_data(std::string_view file);
        static std::string path_assets(std::string_view file);
        static std::string path_engine_data(std::string_view file);

        // These don't need to be reset explicitly
        static std::string user_name;
        static std::string app_name;
        static std::string res_directory;
    };
}
