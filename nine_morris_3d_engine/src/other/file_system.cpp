#include <string>
#include <string_view>
#include <stdexcept>

#include "engine/application_base/platform.hpp"

#if defined(SM_PLATFORM_LINUX)
    #include <pwd.h>
    #include <unistd.h>
    #include <sys/stat.h>
#elif defined(SM_PLATFORM_WINDOWS)
    #include <Windows.h>
    #include <Lmcons.h>
#endif

#include "engine/other/file_system.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    // These don't need to be reset explicitly
    static std::string g_user_name;
    static std::string g_app_name;

#if defined(SM_PLATFORM_LINUX)
        #define USER_DATA_DIRECTORY_PATH(user_name, application_name) \
            ("/home/" + (user_name) + "/." + (application_name) + "/")
        #define ASSETS_DIRECTORY_PATH(application_name) \
            ("/usr/local/share/" + (application_name) + "/")

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

        static std::string get_user_name_impl() noexcept(false) {
            uid_t uid = geteuid();
            struct passwd* pw = getpwuid(uid);

            if (pw == nullptr) {
                throw FileSystem::UserNameError("Could not get user name");
            }

            return std::string(pw->pw_name);
        }

        static void check_and_fix_directories_impl() {
            const std::string path = USER_DATA_DIRECTORY_PATH(g_user_name, g_app_name);

            if (!directory_exists_impl(FileSystem::cut_slash(path))) {
                LOG_DIST_WARNING("Directory `{}` doesn't exist, creating it...", path);

                if (create_directory_impl(path)) {
                    LOG_DIST_INFO("Created directory `{}`", path);
                } else {
                    LOG_DIST_ERROR("Could not create directory `{}`", path);
                }
            }
        }
#elif defined(SM_PLATFORM_WINDOWS)
        #define USER_DATA_DIRECTORY_PATH(user_name, application_name) \
            ("C:\\Users\\" + (user_name) + "\\AppData\\Roaming\\" + (application_name) + "\\")
        #define DOCUMENTS_DIRECTORY_PATH(user_name, application_name) \
            ("C:\\Users\\" + (user_name) + "\\Documents\\" + (application_name) + "\\")

        static bool directory_exists_impl(std::string_view path) {
            WIN32_FIND_DATA find_data;
            HANDLE handle = FindFirstFile(path.data(), &find_data);

            if (handle == INVALID_HANDLE_VALUE) {
                FindClose(handle);
                return false;
            } else {
                if (find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
                    FindClose(handle);
                    return true;
                } else {
                    FindClose(handle);
                    return false;
                }
            }
        }

        static bool create_directory_impl(std::string_view path) {
            const bool success = CreateDirectory(path.data(), nullptr);
            return success;
        }

        static std::string get_user_name_impl() noexcept(false) {
            char user_name[UNLEN + 1];
            DWORD _ = UNLEN + 1;
            const bool success = GetUserName(user_name, &_);

            if (!success) {
                throw FileSystem::UserNameError("Could not get user name");
            }

            return std::string(user_name);
        }

        static void check_and_fix_directories_impl() {
            {
                const std::string path = USER_DATA_DIRECTORY_PATH(g_user_name, g_app_name);

                if (!directory_exists_impl(FileSystem::cut_slash(path))) {
                    LOG_DIST_WARNING("Directory `{}` doesn't exist, creating it...", path);

                    if (create_directory_impl(path)) {
                        LOG_DIST_INFO("Created directory `{}`", path);
                    } else {
                        LOG_DIST_ERROR("Could not create directory `{}`", path);
                    }
                }
            }

            {
                const std::string path = DOCUMENTS_DIRECTORY_PATH(g_user_name, g_app_name);

                if (!directory_exists_impl(FileSystem::cut_slash(path))) {
                    LOG_DIST_WARNING("Directory `{}` doesn't exist, creating it...", path);

                    if (create_directory_impl(path)) {
                        LOG_DIST_INFO("Created directory `{}`", path);
                    } else {
                        LOG_DIST_ERROR("Could not create directory `{}`", path);
                    }
                }
            }
        }
#endif

#ifdef SM_BUILD_DISTRIBUTION
    #if defined(SM_PLATFORM_LINUX)
            static std::string path_for_logs_impl() {
                return USER_DATA_DIRECTORY_PATH(g_user_name, g_app_name);
            }

            static std::string path_for_saved_data_impl() {
                return USER_DATA_DIRECTORY_PATH(g_user_name, g_app_name);
            }

            static std::string path_for_assets_impl() {
                return ASSETS_DIRECTORY_PATH(g_app_name);
            }
    #elif defined(SM_PLATFORM_WINDOWS)
            static std::string path_for_logs_impl() {
                return DOCUMENTS_DIRECTORY_PATH(g_user_name, g_app_name);
            }

            static std::string path_for_saved_data_impl() {
                return USER_DATA_DIRECTORY_PATH(g_user_name, g_app_name);
            }

            static std::string path_for_assets_impl() {
                return {};
            }
    #endif
#else
        // Use relative path for both operating systems
        static std::string path_for_logs_impl() {
            return {};
        }

        static std::string path_for_saved_data_impl() {
            return {};
        }

        static std::string path_for_assets_impl() {
            return {};
        }
#endif

    void FileSystem::initialize_for_applications(std::string_view application_name) noexcept(false) {
        g_user_name = get_user_name();
        g_app_name = application_name;
    }

    bool FileSystem::directory_exists(std::string_view path) {
        return directory_exists_impl(path);
    }

    bool FileSystem::create_directory(std::string_view path) {
        return create_directory_impl(path);
    }

    bool FileSystem::delete_file(std::string_view path) {
        return remove(path.data()) != 0;
    }

    std::string FileSystem::cut_slash(std::string_view path) {
        // Needs to return a new string
        return std::string(path.substr(0, path.length() - 1));
    }

    std::string FileSystem::get_user_name() noexcept(false) {
        return get_user_name_impl();
    }

    void FileSystem::check_and_fix_directories() {
        check_and_fix_directories_impl();
    }

    std::string FileSystem::path_for_logs() {
        return path_for_logs_impl();
    }

    std::string FileSystem::path_for_saved_data() {
        return path_for_saved_data_impl();
    }

    std::string FileSystem::path_for_assets() {
        return path_for_assets_impl();
    }

    std::string FileSystem::path_for_logs(std::string_view file) {
        return path_for_logs_impl() + std::string(file);
    }

    std::string FileSystem::path_for_saved_data(std::string_view file) {
        return path_for_saved_data_impl() + std::string(file);
    }

    std::string FileSystem::path_for_assets(std::string_view file) {
        return path_for_assets_impl() + std::string(file);
    }
}
