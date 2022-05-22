#include <string>

#include "application/platform.h"
#include "nine_morris_3d/paths.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "other/user_data.h"

namespace paths {
    std::string path_for_logs(const char* file) noexcept(false) {
#if defined(NINE_MORRIS_3D_DEBUG)
        // Use relative path for both operating systems
        return std::string(file);
#elif defined(NINE_MORRIS_3D_RELEASE)
    #if defined(NINE_MORRIS_3D_LINUX)
        return user_data::get_user_data_directory_path(APP_NAME) + file;
    #elif defined(NINE_MORRIS_3D_WINDOWS)
        return "C:\\Users\\" + user_data::get_username() + "\\Documents\\" + APP_NAME + "\\" + file;
    #endif
#endif
    }

    std::string path_for_save_and_options(const char* file) noexcept(false) {
#if defined(NINE_MORRIS_3D_DEBUG)
        // Use relative path for both operating systems
        return std::string(file);
#elif defined(NINE_MORRIS_3D_RELEASE)
    #if defined(NINE_MORRIS_3D_LINUX)
        return user_data::get_user_data_directory_path(APP_NAME) + file;
    #elif defined(NINE_MORRIS_3D_WINDOWS)
        return user_data::get_user_data_directory_path(APP_NAME) + file;
    #endif
#endif
    }

    std::string path_for_assets(const char* file_path) {
#if defined(NINE_MORRIS_3D_DEBUG)
        // Use relative path for both operating systems
        return std::string(file_path);
#elif defined(NINE_MORRIS_3D_RELEASE)
    #if defined(NINE_MORRIS_3D_LINUX)
        return std::string("/usr/share/") + APP_NAME + "/" + file_path;
    #elif defined(NINE_MORRIS_3D_WINDOWS)
        // Just use relative path
        return std::string(file_path);
    #endif
#endif
    }
}
