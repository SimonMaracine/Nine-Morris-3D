#pragma once

#include <string>

namespace sm {
    struct FileSystem {
    public:
        FileSystem(const std::string& application_name, const std::string& assets_directory);

        bool directory_exists(const std::string& path) const;
        bool create_directory(const std::string& path) const;
        bool delete_file(const std::string& path) const;

        std::string path_logs() const;
        std::string path_saved_data() const;
        std::string path_assets() const;
        std::string path_engine_assets() const;

        std::string path_logs(const std::string& file) const;
        std::string path_saved_data(const std::string& file) const;
        std::string path_assets(const std::string& file) const;
        std::string path_engine_assets(const std::string& file) const;
    private:
        void check_and_fix_directories() const;

        std::string application_name;
        std::string assets_directory;
        std::string user_name;
    };
}
