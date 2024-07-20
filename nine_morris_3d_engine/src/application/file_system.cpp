#include "nine_morris_3d_engine/application/file_system.hpp"

#include <filesystem>
#include <optional>

#include "nine_morris_3d_engine/application/platform.hpp"

#if defined(SM_PLATFORM_LINUX)
    #include <sys/types.h>
    #include <pwd.h>
    #include <unistd.h>
#elif defined(SM_PLATFORM_WINDOWS)
    #include <Windows.h>
#endif

#include "nine_morris_3d_engine/application/error.hpp"

#if defined(SM_PLATFORM_LINUX)
    #define USER_DATA_DIRECTORY_PATH(user_name, application_name) \
        ("/home/" + (user_name) + "/." + (application_name) + "/")
    #define DATA_DIRECTORY_PATH(application_name) \
        ("/usr/local/share/" + (application_name) + "/")
#elif defined(SM_PLATFORM_WINDOWS)
    #define USER_DATA_DIRECTORY_PATH(user_name, application_name) \
        ("C:\\Users\\" + (user_name) + "\\AppData\\Roaming\\" + (application_name) + "\\")
    #define USER_DATA2_DIRECTORY_PATH(user_name, application_name) \
        ("C:\\Users\\" + (user_name) + "\\Documents\\" + (application_name) + "\\")
#endif

static std::optional<std::string> get_user_name() {
#if defined(SM_PLATFORM_LINUX)
    const uid_t uid {geteuid()};
    struct passwd* pw {getpwuid(uid)};

    if (pw == nullptr) {
        return std::nullopt;
    }

    return std::make_optional(pw->pw_name);
#elif defined(SM_PLATFORM_WINDOWS)
    char user_name[UNLEN + 1] {};
    const DWORD _ {UNLEN + 1};
    const bool success {GetUserName(user_name, &_)};

    if (!success) {
        return std::nullopt;
    }

    return std::make_optional(user_name);
#endif
}

namespace sm {
    namespace internal {
        FileSystem::FileSystem(const std::string& application_name, const std::string& assets_directory)
            : application_name(application_name), assets_directory(assets_directory) {
            const auto name {get_user_name()};

            if (!name) {
                throw InitializationError("Could not retrieve user name");
            }

            user_name = *name;

    #ifdef SM_BUILD_DISTRIBUTION
            check_and_fix_directories();
    #endif
        }

        bool FileSystem::directory_exists(const std::string& path) {
            return std::filesystem::is_directory(path);  // FIXME handle exceptions
        }

        bool FileSystem::create_directory(const std::string& path) {
            return std::filesystem::create_directory(path);
        }

        bool FileSystem::delete_file(const std::string& path) {
            return std::filesystem::remove(path);
        }

        std::string FileSystem::current_working_directory() {
            return std::filesystem::current_path().string();
        }

        std::string FileSystem::path_logs() const {
    #ifdef SM_BUILD_DISTRIBUTION
        #if defined(SM_PLATFORM_LINUX)
            return USER_DATA_DIRECTORY_PATH(user_name, application_name);
        #elif defined(SM_PLATFORM_WINDOWS)
            return USER_DATA2_DIRECTORY_PATH(user_name, application_name);
        #endif
    #else
            return {};
    #endif
        }

        std::string FileSystem::path_saved_data() const {
    #ifdef SM_BUILD_DISTRIBUTION
        #if defined(SM_PLATFORM_LINUX)
            return USER_DATA_DIRECTORY_PATH(user_name, application_name);
        #elif defined(SM_PLATFORM_WINDOWS)
            return USER_DATA_DIRECTORY_PATH(user_name, application_name);
        #endif
    #else
            return {};
    #endif
        }

        std::string FileSystem::path_assets() const {
    #ifdef SM_BUILD_DISTRIBUTION
        #if defined(SM_PLATFORM_LINUX)
            return DATA_DIRECTORY_PATH(application_name) + assets_directory + '/';
        #elif defined(SM_PLATFORM_WINDOWS)
            return assets_directory + '\\';
        #endif
    #else
        #if defined(SM_PLATFORM_LINUX)
            return assets_directory + '/';
        #elif defined(SM_PLATFORM_WINDOWS)
            return assets_directory + '\\';
        #endif
    #endif
        }

        std::string FileSystem::path_engine_assets() const {
    #ifdef SM_BUILD_DISTRIBUTION
        #if defined(SM_PLATFORM_LINUX)
            return DATA_DIRECTORY_PATH(application_name) + "engine_assets/";
        #elif defined(SM_PLATFORM_WINDOWS)
            return "engine_assets/";
        #endif
    #else
            return "engine_assets/";
    #endif
        }

        std::string FileSystem::path_logs(const std::string& path) const {
            return path_logs() + path;
        }

        std::string FileSystem::path_saved_data(const std::string& path) const {
            return path_saved_data() + path;
        }

        std::string FileSystem::path_assets(const std::string& path) const {
            return path_assets() + path;
        }

        std::string FileSystem::path_engine_assets(const std::string& path) const {
            return path_engine_assets() + path;
        }

        void FileSystem::check_and_fix_directories() const {
    #if defined(SM_PLATFORM_LINUX)
            const std::string path {USER_DATA_DIRECTORY_PATH(user_name, application_name)};

            if (!directory_exists(path)) {
                error_string = "Directory `" + path + "` doesn't exist, creating it...";

                if (!create_directory(path)) {
                    throw InitializationError("Could not create directory `" + path + "`");
                }
            }
    #elif defined(SM_PLATFORM_WINDOWS)
            std::string path;

            path = USER_DATA_DIRECTORY_PATH(user_name, application_name);

            if (!directory_exists(path)) {
                error_string + "Directory `" + path + "` doesn't exist, creating it...";

                if (!create_directory(path)) {
                    throw InitializationError("Could not create directory `" + path + "`");
                }
            }

            path = USER_DATA2_DIRECTORY_PATH(user_name, application_name);

            if (!directory_exists(path)) {
                error_string = "Directory `" + path + "` doesn't exist, creating it...";

                if (!create_directory(path)) {
                    throw InitializationError("Could not create directory `" + path + "`");
                }
            }
    #endif
        }
    }
}
