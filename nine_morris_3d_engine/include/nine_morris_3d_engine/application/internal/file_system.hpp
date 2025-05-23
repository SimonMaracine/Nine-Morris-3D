#pragma once

#include <string>
#include <filesystem>

namespace sm::internal {
    // File system API
    // Stores game-related paths
    class FileSystem {
    public:
        FileSystem(
            const std::filesystem::path& path_logs,
            const std::filesystem::path& path_saved_data,
            const std::filesystem::path& path_assets,
            const std::filesystem::path& assets_directory
        );

        // Basic file operations
        static bool file_exists(const std::filesystem::path& path);
        static bool is_directory(const std::filesystem::path& path);
        static bool create_directory(const std::filesystem::path& path);
        static bool delete_file(const std::filesystem::path& path);
        static std::filesystem::path current_working_directory();

        // Retrieve paths
        std::filesystem::path path_logs() const;
        std::filesystem::path path_saved_data() const;
        std::filesystem::path path_assets() const;
        std::filesystem::path path_engine_assets() const;

        // Retrieve concatenated paths
        std::filesystem::path path_logs(const std::filesystem::path& path) const;
        std::filesystem::path path_saved_data(const std::filesystem::path& path) const;
        std::filesystem::path path_assets(const std::filesystem::path& path) const;
        std::filesystem::path path_engine_assets(const std::filesystem::path& path) const;

        // Verify if the directory paths exist and create them if necessary
        void check_and_fix_directories() const;

        // Clear and get the last error string
        std::string get_error_string() const;
    private:
        void check_directory(const std::filesystem::path& path) const;
        static bool no_directory(const std::filesystem::path& path);

        std::filesystem::path m_path_logs;
        std::filesystem::path m_path_saved_data;
        std::filesystem::path m_path_assets;
        std::filesystem::path m_assets_directory;

        mutable std::string m_error_string;
    };
}
