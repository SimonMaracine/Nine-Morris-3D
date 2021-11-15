#include <string>
#include <string.h>

#if defined(__GNUG__)
    #include <pwd.h>
    #include <unistd.h>
    #include <sys/stat.h>
#elif defined(_MSC_VER)
    // TODO Windows API
#else
    #error "GCC or MSVC must be used (for now)"
#endif

#include "other/logging.h"

#define APP_NAME ".ninemorris3d"

namespace user_data {
#if defined(__GNUG__)
    std::string get_username() {
        uid_t uid = geteuid();
        struct passwd* pw = getpwuid(uid);

        if (pw == nullptr) {
            spdlog::error("Could not get username");
            return std::string("");
        }

        return std::string(pw->pw_name);
    }

    std::string get_user_data_path() {
        std::string path = "/home/" + get_username() + "/" + APP_NAME;

        return path;
    }

    bool user_data_directory_exists() {
        std::string path = "/home/" + get_username() + "/" + APP_NAME;

        struct stat sb;

        if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return true;
        } else {
            return false;
        }
    }

    bool create_user_data_directory() {
        std::string path = "/home/" + get_username() + "/" + APP_NAME;

        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0) {
            return false;
        } else {
            return true;
        }
    }
#elif defined(_MSC_VER)
    // TODO implement these

    std::string get_username() {
        return std::string();
    }

    std::string get_user_data_path() {
        return std::string();
    }

    bool user_data_directory_exists() {
        return false;
    }

    bool create_user_data_directory() {
        return false;
    }
#endif
}
