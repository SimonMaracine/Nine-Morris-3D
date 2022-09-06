#include "nine_morris_3d_engine/application/platform.h"

#if defined(PLATFORM_GAME_LINUX)
    #include <pwd.h>
    #include <unistd.h>
    #include <sys/stat.h>
#elif defined(PLATFORM_GAME_WINDOWS)
    #include <Windows.h>
    #include <Lmcons.h>
#endif

#include "nine_morris_3d_engine/other/user_data.h"
#include "nine_morris_3d_engine/other/logging.h"

namespace user_data {
#if defined(PLATFORM_GAME_LINUX)
    #define DIRECTORY_PATH(username, app_name) ("/home/" + (username) + "/." + (app_name) + "/")

    std::string get_username() noexcept(false) {
        uid_t uid = geteuid();
        struct passwd* pw = getpwuid(uid);

        if (pw == nullptr) {
            throw UserNameError("Could not get username");
        }

        return std::string(pw->pw_name);
    }

    std::string get_user_data_directory_path(std::string_view application_name) noexcept(false) {
        const std::string username = get_username();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        return path;
    }

    bool user_data_directory_exists(std::string_view application_name) noexcept(false) {
        const std::string username = get_username();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        struct stat sb;

        if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return true;
        } else {
            return false;
        }
    }

    bool create_user_data_directory(std::string_view application_name) noexcept(false) {
        const std::string username = get_username();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0) {
            return false;
        } else {
            return true;
        }
    }
#elif defined(PLATFORM_GAME_WINDOWS)
    #define DIRECTORY_PATH(username, app_name) ("C:\\Users\\" + (username) + "\\AppData\\Roaming\\" + (app_name) + "\\")

    std::string get_username() noexcept(false) {
        char username[UNLEN + 1];
        DWORD whatever = UNLEN + 1;
        const bool success = GetUserName(username, &whatever);

        if (!success) {
            throw UserNameError("Could not get username");
        }

        return std::string(username);
    }

    std::string get_user_data_directory_path(std::string_view application_name) noexcept(false) {
        const std::string username = get_username();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        return path;
    }

    bool user_data_directory_exists(std::string_view application_name) noexcept(false) {
        const std::string username = get_username();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

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

    bool create_user_data_directory(std::string_view application_name) noexcept(false) {
        const std::string username = get_username();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        const bool success = CreateDirectory(path.c_str(), nullptr);

        return success;
    }
#endif
}
