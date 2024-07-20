#pragma once

#include <string>

namespace sm {
    namespace internal {
        class FileSystem {
        public:
            FileSystem(const std::string& application_name, const std::string& assets_directory);

            static bool directory_exists(const std::string& path);
            static bool create_directory(const std::string& path);
            static bool delete_file(const std::string& path);
            static std::string current_working_directory();

            std::string path_logs() const;
            std::string path_saved_data() const;
            std::string path_assets() const;
            std::string path_engine_assets() const;

            std::string path_logs(const std::string& path) const;
            std::string path_saved_data(const std::string& path) const;
            std::string path_assets(const std::string& path) const;
            std::string path_engine_assets(const std::string& path) const;

            void check_and_fix_directories() const;
            const std::string& get_error_string() const { return error_string; }
        private:
            std::string application_name;
            std::string assets_directory;
            std::string user_name;

            mutable std::string error_string;
        };
    }
}
