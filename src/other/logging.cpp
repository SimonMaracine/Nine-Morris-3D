#include <fstream>
#include <string>
#include <exception>

#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "graphics/debug_opengl.h"
#include "other/logging.h"
#include "other/user_data.h"

#define LOG_FILE "log.txt"
#define INFO_FILE "info.txt"

#define LOG_PATTERN "%^[%l] [th %t] [%H:%M:%S]%$ %v"

/* SPDLOG_TRACE, SPDLOG_DEBUG, SPDLOG_INFO,
SPDLOG_WARN, SPDLOG_ERROR, SPDLOG_CRITICAL */

namespace logging {
    std::shared_ptr<spdlog::logger> release_logger;

    static std::string path(const char* file) {  // Throws exception
#ifndef NDEBUG
        // Use relative path for both operating systems
        return std::string(file);
#else
    #if defined(__GNUG__)
        std::string path = user_data::get_user_data_directory_path() + file;
        return path;
    #elif defined(_MSC_VER)
        std::string path = "C:\\Users\\" + user_data::get_username() + "\\Documents\\" + APP_NAME_WINDOWS + "\\" + file;
        return path;
    #else
        #error "GCC or MSVC must be used (for now)"
    #endif
#endif
    }

    void initialize() {
        spdlog::set_pattern(LOG_PATTERN);
        spdlog::set_level(spdlog::level::trace);

        std::string file_path;
        try {
            file_path = path(LOG_FILE);
        } catch (const std::runtime_error& e) {            
            release_logger = spdlog::stdout_color_mt("Release Logger Fallback (Console)");

            release_logger->set_pattern(LOG_PATTERN);
            release_logger->set_level(spdlog::level::trace);

            spdlog::error("Using fallback logger (console)");
            return;
        }

        try {
            release_logger = spdlog::basic_logger_mt("Release Logger (File)", file_path, false);
        } catch (const spdlog::spdlog_ex& e) {
            release_logger = spdlog::stdout_color_mt("Release Logger Fallback (Console)");

            release_logger->set_pattern(LOG_PATTERN);
            release_logger->set_level(spdlog::level::trace);

            spdlog::error("Using fallback logger (console): {}", e.what());
            return;
        }

        release_logger->set_pattern(LOG_PATTERN);
        release_logger->set_level(spdlog::level::trace);
        release_logger->flush_on(spdlog::level::info);
    }

    void log_opengl_and_dependencies_info(LogTarget target) {
        const std::string contents = debug_opengl::get_info();

        switch (target) {
            case LogTarget::File: {
                std::string file_path;
                try {
                    file_path = path(INFO_FILE);
                } catch (const std::runtime_error& e) {
                    REL_ERROR("{}", e.what());
                    break;
                }

                std::ofstream file (file_path, std::ios::trunc);

                if (!file.is_open()) {
                    REL_ERROR("Could not open file '{}' for writing", file_path.c_str());
                    break;
                }

                file << contents.c_str();

                break;
            }
            case LogTarget::Console:
                REL_INFO("{}", contents.c_str());

                break;
            case LogTarget::None:
                // Do nothing
                break;
        }
    }
}
