#include "engine/application/platform.h"
#include "engine/other/path.h"
#include "engine/other/user_data.h"

static std::string _app_name;

#if defined(NM3D_PLATFORM_DEBUG)
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
#elif defined(NM3D_PLATFORM_RELEASE)
    #if defined(NM3D_PLATFORM_LINUX)
        static std::string path_for_logs_impl(std::string_view file) noexcept(false) {
            return user_data::get_user_data_directory_path(_app_name) + std::string(file);
        }

        static std::string path_for_saved_data_impl(std::string_view file) noexcept(false) {
            return user_data::get_user_data_directory_path(_app_name) + std::string(file);
        }

        static std::string path_for_assets_impl(std::string_view file_path) {
            return std::string("/usr/local/share/") + _app_name + "/" + std::string(file_path);
        }
    #elif defined(NM3D_PLATFORM_WINDOWS)
        static std::string path_for_logs_impl(std::string_view file) noexcept(false) {
            return "C:\\Users\\" + user_data::get_username() + "\\Documents\\" + _app_name + "\\" + std::string(file);
        }

        static std::string path_for_saved_data_impl(std::string_view file) noexcept(false) {
            return user_data::get_user_data_directory_path(_app_name) + std::string(file);
        }

        static std::string path_for_assets_impl(std::string_view file_path) {
            return std::string(file_path);
        }
    #endif
#endif

namespace path {
    void initialize_for_applications(std::string_view application_name) {
        _app_name = application_name;
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
