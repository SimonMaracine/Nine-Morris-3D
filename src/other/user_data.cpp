#include <string>
#include <exception>

#if defined(__GNUG__)
    #include <pwd.h>
    #include <unistd.h>
    #include <sys/stat.h>
#elif defined(_MSC_VER)
    #include <Windows.h>
    #include <Lmcons.h>
#else
    #error "GCC or MSVC must be used (for now)"
#endif

#include "other/logging.h"

#define APP_NAME_LINUX ".ninemorris3d"
#define APP_NAME_WINDOWS "NineMorris3D"

namespace user_data {
#if defined(__GNUG__)
    const std::string get_username() {
        uid_t uid = geteuid();
        struct passwd* pw = getpwuid(uid);

        if (pw == nullptr) {
            throw std::runtime_error("Could not get username");
        }

        return std::string(pw->pw_name);
    }

    const std::string get_user_data_path() {
        const std::string username = get_username();
        const std::string path = "/home/" + username + "/" + APP_NAME_LINUX;

        return path;
    }

    bool user_data_directory_exists() {
        const std::string username = get_username();
        const std::string path = "/home/" + username + "/" + APP_NAME_LINUX;

        struct stat sb;

        if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return true;
        } else {
            return false;
        }
    }

    bool create_user_data_directory() {
        const std::string username = get_username();
        const std::string path = "/home/" + username + "/" + APP_NAME_LINUX;

        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0) {
            return false;
        } else {
            return true;
        }
    }
#elif defined(_MSC_VER)
    const std::string get_username() {
        char username[UNLEN + 1];
        DWORD whatever = UNLEN + 1;
        bool success = GetUserName(username, &whatever);

        if (!success) {
            throw std::runtime_error("Could not get username");
        }

        return std::string(username);
    }

    const std::string get_user_data_path() {
        const std::string username = get_username();
        const std::string path = "C:\\Users\\" + username + "\\AppData\\Roaming\\" + APP_NAME_WINDOWS;

        return path;
    }

    bool user_data_directory_exists() {
        const std::string username = get_username();
        const std::string path = "C:\\Users\\" + username + "\\AppData\\Roaming\\" + APP_NAME_WINDOWS;

        WIN32_FIND_DATA find_data;
        HANDLE find_handle = FindFirstFile(path.c_str(), &find_data);

        if (find_handle == INVALID_HANDLE_VALUE) {
            FindClose(find_handle);
            return false;
        } else {
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                FindClose(find_handle);
                return true;
            } else {
                FindClose(find_handle);
                return false;
            }
        }
    }

    bool create_user_data_directory() {
        const std::string username = get_username();
        const std::string path = "C:\\Users\\" + username + "\\AppData\\Roaming\\" + APP_NAME_WINDOWS;

        bool success = CreateDirectory(path.c_str(), nullptr);

        return success;
    }
#endif
}
