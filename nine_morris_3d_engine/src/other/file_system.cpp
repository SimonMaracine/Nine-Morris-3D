#include "engine/other/file_system.hpp"

#include "engine/application_base/platform.hpp"

#if defined(SM_PLATFORM_LINUX)
    #include <pwd.h>
    #include <unistd.h>
    #include <sys/stat.h>
#elif defined(SM_PLATFORM_WINDOWS)
    #include <Windows.h>
    #include <Lmcons.h>
#endif

#include "engine/other/logging.hpp"

namespace sm {
#if defined(SM_PLATFORM_LINUX)
    #define USER_DATA_DIRECTORY_PATH(user_name, application_name) \
        ("/home/" + (user_name) + "/." + (application_name) + "/")
    #define ASSETS_DIRECTORY_PATH(application_name) \
        ("/usr/local/share/" + (application_name) + "/")

        static bool directory_exists_impl(const std::string& path) {
            struct stat sb;

            if (stat(path.c_str(), &sb) == 0 && S_ISDIR(sb.st_mode)) {
                return true;
            } else {
                return false;
            }
        }

        static bool create_directory_impl(const std::string& path) {
            if (mkdir(path.c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) < 0) {
                return false;
            } else {
                return true;
            }
        }

        static std::optional<std::string> get_user_name_impl() {
            const uid_t uid {geteuid()};
            struct passwd* pw {getpwuid(uid)};

            if (pw == nullptr) {
                return std::nullopt;
            }

            return std::make_optional(pw->pw_name);
        }

        static void check_and_fix_directories_impl() {
            const std::string path {USER_DATA_DIRECTORY_PATH(FileSystem::user_name, FileSystem::app_name)};

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

        static bool directory_exists_impl(const std::string& path) {
            WIN32_FIND_DATA find_data;
            HANDLE handle {FindFirstFile(path.c_str(), &find_data)};

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

        static bool create_directory_impl(const std::string& path) {
            const bool success {CreateDirectory(path.c_str(), nullptr)};
            return success;
        }

        static std::optional<std::string> get_user_name_impl() {
            char user_name[UNLEN + 1];
            DWORD _ {UNLEN + 1};
            const bool success {GetUserName(user_name, &_)};

            if (!success) {
                return std::nullopt;
            }

            return std::make_optional(user_name);
        }

        static void check_and_fix_directories_impl() {
            std::string path;

            path = USER_DATA_DIRECTORY_PATH(FileSystem::user_name, FileSystem::app_name);

            if (!directory_exists_impl(FileSystem::cut_slash(path))) {
                LOG_DIST_WARNING("Directory `{}` doesn't exist, creating it...", path);

                if (create_directory_impl(path)) {
                    LOG_DIST_INFO("Created directory `{}`", path);
                } else {
                    LOG_DIST_ERROR("Could not create directory `{}`", path);
                }
            }

            path = DOCUMENTS_DIRECTORY_PATH(FileSystem::user_name, FileSystem::app_name);

            if (!directory_exists_impl(FileSystem::cut_slash(path))) {
                LOG_DIST_WARNING("Directory `{}` doesn't exist, creating it...", path);

                if (create_directory_impl(path)) {
                    LOG_DIST_INFO("Created directory `{}`", path);
                } else {
                    LOG_DIST_ERROR("Could not create directory `{}`", path);
                }
            }
        }
#endif

#ifdef SM_BUILD_DISTRIBUTION
    #if defined(SM_PLATFORM_LINUX)
            static std::string path_logs_impl() {
                return USER_DATA_DIRECTORY_PATH(FileSystem::user_name, FileSystem::app_name);
            }

            static std::string path_saved_data_impl() {
                return USER_DATA_DIRECTORY_PATH(FileSystem::user_name, FileSystem::app_name);
            }

            static std::string path_assets_impl() {
                return ASSETS_DIRECTORY_PATH(FileSystem::app_name);
            }
    #elif defined(SM_PLATFORM_WINDOWS)
            static std::string path_logs_impl() {
                return DOCUMENTS_DIRECTORY_PATH(FileSystem::user_name, FileSystem::app_name);
            }

            static std::string path_saved_data_impl() {
                return USER_DATA_DIRECTORY_PATH(FileSystem::user_name, FileSystem::app_name);
            }

            static std::string path_assets_impl() {
                return {};
            }
    #endif
#else
        // Use relative paths for both operating systems
        static std::string path_logs_impl() {
            return {};
        }

        static std::string path_saved_data_impl() {
            return {};
        }

        static std::string path_assets_impl() {
            return {};
        }
#endif

    bool FileSystem::initialize_applications(const std::string& app_name, const std::string& res_directory) {
        const auto username {get_user_name()};

        if (!username) {
            return false;
        }

        user_name = *username;
        FileSystem::app_name = app_name;
        FileSystem::res_directory = res_directory;

        return true;
    }

    bool FileSystem::directory_exists(const std::string& path) {
        return directory_exists_impl(path);
    }

    bool FileSystem::create_directory(const std::string& path) {
        return create_directory_impl(path);
    }

    bool FileSystem::delete_file(const std::string& path) {
        return remove(path.c_str()) != 0;
    }

    std::string FileSystem::cut_slash(const std::string& path) {
        return path.substr(0, path.length() - 1);
    }

    std::optional<std::string> FileSystem::get_user_name() {
        return get_user_name_impl();
    }

    void FileSystem::check_and_fix_directories() {
        check_and_fix_directories_impl();
    }

    std::string FileSystem::path_logs() {
        return path_logs_impl();
    }

    std::string FileSystem::path_saved_data() {
        return path_saved_data_impl();
    }

    std::string FileSystem::path_assets() {
        return path_assets_impl() + FileSystem::res_directory + '/';
    }

    std::string FileSystem::path_engine_assets() {
        return path_assets_impl() + "engine_assets/";
    }

    std::string FileSystem::path_logs(const std::string& file) {
        return path_logs_impl() + file;
    }

    std::string FileSystem::path_saved_data(const std::string& file) {
        return path_saved_data_impl() + file;
    }

    std::string FileSystem::path_assets(const std::string& file) {
        return path_assets_impl() + FileSystem::res_directory + '/' + file;
    }

    std::string FileSystem::path_engine_assets(const std::string& file) {
        return path_assets_impl() + "engine_assets/" + file;
    }

    std::string FileSystem::user_name;
    std::string FileSystem::app_name;
    std::string FileSystem::res_directory;
}
