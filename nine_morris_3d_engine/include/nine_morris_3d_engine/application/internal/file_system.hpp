#pragma once

#include <string>
#include <filesystem>

namespace sm::internal {
    class FileSystem {
    public:
        FileSystem(
            const std::string& path_logs,
            const std::string& path_saved_data,
            const std::string& path_assets,
            const std::string& assets_directory
        );

        static bool file_exists(const std::string& path);
        static bool is_directory(const std::string& path);
        static bool create_directory(const std::string& path);
        static bool delete_file(const std::string& path);
        static std::filesystem::path current_working_directory();

        std::filesystem::path path_logs() const;
        std::filesystem::path path_saved_data() const;
        std::filesystem::path path_assets() const;
        std::filesystem::path path_engine_assets() const;

        std::filesystem::path path_logs(const std::string& path) const;
        std::filesystem::path path_saved_data(const std::string& path) const;
        std::filesystem::path path_assets(const std::string& path) const;
        std::filesystem::path path_engine_assets(const std::string& path) const;

        void check_and_fix_directories() const;
        const std::string& get_error_string() const noexcept;
    private:
        void check_directory(const std::string& path) const;
        static bool no_directory(const std::string& path);

        std::string m_path_logs;
        std::string m_path_saved_data;
        std::string m_path_assets;
        std::string m_assets_directory;

        mutable std::string m_error_string;
    };
}
