#include "nine_morris_3d_engine/application/internal/file_system.hpp"

#include <utility>

#include "nine_morris_3d_engine/application/internal/error.hpp"
#include "nine_morris_3d_engine/application/platform.hpp"

namespace sm::internal {
    FileSystem::FileSystem(
        const std::filesystem::path& path_logs,
        const std::filesystem::path& path_saved_data,
        const std::filesystem::path& path_assets,
        const std::filesystem::path& assets_directory
    )
        : m_path_logs(path_logs),
        m_path_saved_data(path_saved_data),
        m_path_assets(path_assets),
        m_assets_directory(assets_directory) {
#ifdef SM_BUILD_DISTRIBUTION
        check_and_fix_directories();
#endif
    }

    bool FileSystem::file_exists(const std::filesystem::path& path) {
        std::error_code ec;
        const bool result {std::filesystem::exists(path, ec)};

        if (ec) {
            throw OtherError("Could not check if file `" + path.string() + "` exists: " + ec.message());
        }

        return result;
    }

    bool FileSystem::is_directory(const std::filesystem::path& path) {
        std::error_code ec;
        const bool result {std::filesystem::is_directory(path, ec)};

        if (ec) {
            throw OtherError("Could not check if file `" + path.string() + "` is a directory: " + ec.message());
        }

        return result;
    }

    bool FileSystem::create_directory(const std::filesystem::path& path) {
        std::error_code ec;
        const bool result {std::filesystem::create_directory(path, ec)};

        if (ec) {
            throw OtherError("Could not create directory `" + path.string() + "`: " + ec.message());
        }

        return result;
    }

    bool FileSystem::delete_file(const std::filesystem::path& path) {
        std::error_code ec;
        const bool result {std::filesystem::remove(path, ec)};

        if (ec) {
            throw OtherError("Could not remove file or directory `" + path.string() + "`: " + ec.message());
        }

        return result;
    }

    std::filesystem::path FileSystem::current_working_directory() {
        std::error_code ec;
        const auto path {std::filesystem::current_path(ec)};

        if (ec) {
            throw OtherError("Could not retrieve current working directory: " + ec.message());
        }

        return path;
    }

    std::filesystem::path FileSystem::path_logs() const {
        return m_path_logs;
    }

    std::filesystem::path FileSystem::path_saved_data() const {
        return m_path_saved_data;
    }

    std::filesystem::path FileSystem::path_assets() const {
        return m_path_assets / m_assets_directory;
    }

    std::filesystem::path FileSystem::path_engine_assets() const {
        return m_path_assets / "assets_engine";
    }

    std::filesystem::path FileSystem::path_logs(const std::filesystem::path& path) const {
        return path_logs() / path;
    }

    std::filesystem::path FileSystem::path_saved_data(const std::filesystem::path& path) const {
        return path_saved_data() / path;
    }

    std::filesystem::path FileSystem::path_assets(const std::filesystem::path& path) const {
        return path_assets() / path;
    }

    std::filesystem::path FileSystem::path_engine_assets(const std::filesystem::path& path) const {
        return path_engine_assets() / path;
    }

    void FileSystem::check_and_fix_directories() const {
        check_directory(m_path_logs);
        check_directory(m_path_saved_data);
    }

    std::string FileSystem::get_error_string() const {
        return std::exchange(m_error_string, "");
    }

    void FileSystem::check_directory(const std::filesystem::path& path) const {
        if (no_directory(path)) {
            m_error_string = "Directory `" + path.string() + "` doesn't exist, creating it...";

            if (!create_directory(path)) {
                throw OtherError("Could not create directory `" + path.string() + "`");
            }
        }
    }

#ifdef __GNUG__
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wparentheses"
#endif

    bool FileSystem::no_directory(const std::filesystem::path& path) {
        return !file_exists(path) || file_exists(path) && !is_directory(path);
    }

#ifdef __GNUG__
    #pragma GCC diagnostic pop
#endif
}
