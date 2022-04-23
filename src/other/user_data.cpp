#include <string>

#include "application/platform.h"

#if defined(NINE_MORRIS_3D_LINUX)
    #include <pwd.h>
    #include <unistd.h>
    #include <sys/stat.h>
#elif defined(NINE_MORRIS_3D_WINDOWS)
    #include <Windows.h>
    #include <Lmcons.h>
#endif

#include "other/user_data.h"
#include "other/logging.h"

namespace user_data {
#if defined(NINE_MORRIS_3D_LINUX)
    const std::string get_username() noexcept(false) {
        uid_t uid = geteuid();
        struct passwd* pw = getpwuid(uid);

        if (pw == nullptr) {
            throw UserNameError("Could not get username");
        }

        return std::string(pw->pw_name);
    }

    const std::string get_user_data_directory_path() noexcept(false) {
        const std::string username = get_username();
        const std::string path = "/home/" + username + "/." + APP_NAME_LINUX + "/";

        return path;
    }

    bool user_data_directory_exists() noexcept(false) {
        const std::string username = get_username();
        const std::string path = "/home/" + username + "/." + APP_NAME_LINUX + "/";

        struct stat sb;

        if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return true;
        } else {
            return false;
        }
    }

    bool create_user_data_directory() noexcept(false) {
        const std::string username = get_username();
        const std::string path = "/home/" + username + "/." + APP_NAME_LINUX + "/";

        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0) {
            return false;
        } else {
            return true;
        }
    }
#elif defined(NINE_MORRIS_3D_WINDOWS)
    const std::string get_username() noexcept(false) {
        char username[UNLEN + 1];
        DWORD whatever = UNLEN + 1;
        bool success = GetUserName(username, &whatever);

        if (!success) {
            throw UserNameError("Could not get username");
        }

        return std::string(username);
    }

    const std::string get_user_data_directory_path() noexcept(false) {
        const std::string username = get_username();
        const std::string path = "C:\\Users\\" + username + "\\AppData\\Roaming\\" + APP_NAME_WINDOWS;

        return path;
    }

    bool user_data_directory_exists() noexcept(false) {
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

    bool create_user_data_directory() noexcept(false) {
        const std::string username = get_username();
        const std::string path = "C:\\Users\\" + username + "\\AppData\\Roaming\\" + APP_NAME_WINDOWS;

        bool success = CreateDirectory(path.c_str(), nullptr);

        return success;
    }
#endif
}
