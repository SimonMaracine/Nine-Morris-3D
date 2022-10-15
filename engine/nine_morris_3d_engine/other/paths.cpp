#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/other/paths.h"
#include "nine_morris_3d_engine/other/user_data.h"

#if defined(PLATFORM_GAME_DEBUG)
    // Use relative path for both operating systems
    static std::string path_for_logs_impl(std::string_view file) noexcept(false) {
        return std::string(file);
    }

    static std::string path_for_saved_data_impl(std::string_view file) noexcept(false) {
        return std::string(file);
    }

    static std::string path_for_assets_impl(std::string_view file_path) {
        return std::string(file_path);
    }
#elif defined(PLATFORM_GAME_RELEASE)
    #if defined(PLATFORM_GAME_LINUX)
        static std::string path_for_logs_impl(std::string_view file) noexcept(false) {
            return user_data::get_user_data_directory_path(app_name) + std::string(file);
        }

        static std::string path_for_saved_data_impl(std::string_view file) noexcept(false) {
            return user_data::get_user_data_directory_path(app_name) + std::string(file);
        }

        static std::string path_for_assets_impl(std::string_view file_path) {
            return std::string("/usr/share/") + app_name + "/" + std::string(file_path);
        }
    #elif defined(PLATFORM_GAME_WINDOWS)
        static std::string path_for_logs_impl(std::string_view file) noexcept(false) {
            return "C:\\Users\\" + user_data::get_username() + "\\Documents\\" + app_name + "\\" + std::string(file);
        }

        static std::string path_for_saved_data_impl(std::string_view file) noexcept(false) {
            return user_data::get_user_data_directory_path(app_name) + std::string(file);
        }

        static std::string path_for_assets_impl(std::string_view file_path) {
            return std::string(file_path);
        }
    #endif
#endif

namespace paths {
    static std::string app_name;

    void initialize(std::string_view application_name) {
        app_name = application_name;
    }

    std::string path_for_logs(std::string_view file) noexcept(false) {
        return path_for_logs_impl(file);
    }

    std::string path_for_saved_data(std::string_view file) noexcept(false) {
        return path_for_saved_data_impl(file);
    }

    std::string path_for_assets(std::string_view file_path) {
        return path_for_assets_impl(file_path);
    }
}
