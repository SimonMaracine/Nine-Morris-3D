#include "engine/application/platform.h"

#if defined(NM3D_PLATFORM_LINUX)
    #include <pwd.h>
    #include <unistd.h>
    #include <sys/stat.h>
#elif defined(NM3D_PLATFORM_WINDOWS)
    #include <Windows.h>
    #include <Lmcons.h>
#endif

#include "engine/other/user_data.h"
#include "engine/other/logging.h"

#if defined(NM3D_PLATFORM_LINUX)
    #define DIRECTORY_PATH(username, app_name) ("/home/" + (username) + "/." + (app_name) + "/")

    static bool directory_exists_impl(std::string_view path) {
        struct stat sb;

        if (stat(path.data(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
            return true;
        } else {
            return false;
        }
    }

    static bool create_directory_impl(std::string_view path) {
        if (mkdir(path.data(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0) {
            return false;
        } else {
            return true;
        }
    }

    static std::string get_username_impl() noexcept(false) {
        uid_t uid = geteuid();
        struct passwd* pw = getpwuid(uid);

        if (pw == nullptr) {
            throw user_data::UserNameError("Could not get username");
        }

        return std::string(pw->pw_name);
    }

    static std::string get_user_data_directory_path_impl(std::string_view application_name) noexcept(false) {
        const std::string username = get_username_impl();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        return path;
    }

    static bool user_data_directory_exists_impl(std::string_view application_name) noexcept(false) {
        const std::string username = get_username_impl();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        return directory_exists_impl(path);
    }

    static bool create_user_data_directory_impl(std::string_view application_name) noexcept(false) {
        const std::string username = get_username_impl();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        return create_directory_impl(path);
    }
#elif defined(NM3D_PLATFORM_WINDOWS)
    #define DIRECTORY_PATH(username, app_name) ("C:\\Users\\" + (username) + "\\AppData\\Roaming\\" + (app_name) + "\\")

    static bool directory_exists_impl(std::string_view path) {
        WIN32_FIND_DATA find_data;
        HANDLE find_handle = FindFirstFile(path.data(), &find_data);

        if (find_handle == INVALID_HANDLE_VALUE) {
            FindClose(find_handle);
            return false;
        } else {
            if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                FindClose(find_handle);
                return true;
            }
            else {
                FindClose(find_handle);
                return false;
            }
        }
    }

    static bool create_directory_impl(std::string_view path) {
        const bool success = CreateDirectory(path.data(), nullptr);
        return success;
    }

    static std::string get_username_impl() noexcept(false) {
        char username[UNLEN + 1];
        DWORD _ = UNLEN + 1;
        const bool success = GetUserName(username, &_);

        if (!success) {
            throw user_data::UserNameError("Could not get username");
        }

        return std::string(username);
    }

    static std::string get_user_data_directory_path_impl(std::string_view application_name) noexcept(false) {
        const std::string username = get_username_impl();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        return path;
    }

    static bool user_data_directory_exists_impl(std::string_view application_name) noexcept(false) {
        const std::string username = get_username_impl();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        return directory_exists_impl(path);
    }

    static bool create_user_data_directory_impl(std::string_view application_name) noexcept(false) {
        const std::string username = get_username_impl();
        const std::string path = DIRECTORY_PATH(username, std::string(application_name));

        return create_directory_impl(path);
    }
#endif

namespace user_data {
    bool directory_exists(std::string_view path) {
        return directory_exists_impl(path);
    }

    bool create_directory(std::string_view path) {
        return create_directory_impl(path);
    }

    std::string get_username() noexcept(false) {
        return get_username_impl();
    }

    std::string get_user_data_directory_path(std::string_view application_name) noexcept(false) {
        return get_user_data_directory_path_impl(application_name);
    }

    bool user_data_directory_exists(std::string_view application_name) noexcept(false) {
        return user_data_directory_exists_impl(application_name);
    }

    bool create_user_data_directory(std::string_view application_name) noexcept(false) {
        return create_user_data_directory_impl(application_name);
    }
}
