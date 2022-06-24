#include "application/platform.h"
#include "other/paths.h"
#include "other/user_data.h"

namespace paths {
    static std::string app_name;

    void initialize(std::string_view application_name) {
        app_name = std::string(application_name);
    }

    std::string path_for_logs(std::string_view file) noexcept(false) {
#if defined(PLATFORM_GAME_DEBUG)
        // Use relative path for both operating systems
        return std::string(file);
#elif defined(PLATFORM_GAME_RELEASE)
    #if defined(PLATFORM_GAME_LINUX)
        return user_data::get_user_data_directory_path(app_name) + std::string(file);
    #elif defined(PLATFORM_GAME_WINDOWS)
        return "C:\\Users\\" + user_data::get_username() + "\\Documents\\" + app_name + "\\" + std::string(file);
    #endif
#endif
    }

    std::string path_for_saved_data(std::string_view file) noexcept(false) {
#if defined(PLATFORM_GAME_DEBUG)
        // Use relative path for both operating systems
        return std::string(file);
#elif defined(PLATFORM_GAME_RELEASE)
    #if defined(PLATFORM_GAME_LINUX)
        return user_data::get_user_data_directory_path(app_name) + std::string(file);
    #elif defined(PLATFORM_GAME_WINDOWS)
        return user_data::get_user_data_directory_path(app_name) + std::string(file);
    #endif
#endif
    }

    std::string path_for_assets(std::string_view file_path) {
#if defined(PLATFORM_GAME_DEBUG)
        // Use relative path for both operating systems
        return std::string(file_path);
#elif defined(PLATFORM_GAME_RELEASE)
    #if defined(PLATFORM_GAME_LINUX)
        return std::string("/usr/share/") + app_name + "/" + std::string(file_path);
    #elif defined(PLATFORM_GAME_WINDOWS)
        // Just use relative path
        return std::string(file_path);
    #endif
#endif
    }
}
