#pragma once

#include <string>
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
        static void initialize_applications(const std::string& app_name, const std::string& res_directory) noexcept(false);

        static bool directory_exists(const std::string& path);  // Path must not end with trailing backslash
        static bool create_directory(const std::string& path);
        static bool delete_file(const std::string& path);

        static std::string cut_slash(const std::string& path);

        static std::string get_user_name() noexcept(false);
        static void check_and_fix_directories();

        static std::string path_logs();
        static std::string path_saved_data();
        static std::string path_assets();
        static std::string path_engine_data();

        static std::string path_logs(const std::string& file);
        static std::string path_saved_data(const std::string& file);
        static std::string path_assets(const std::string& file);
        static std::string path_engine_data(const std::string& file);

        // These don't need to be reset explicitly
        static std::string user_name;
        static std::string app_name;
        static std::string res_directory;
    };
}
