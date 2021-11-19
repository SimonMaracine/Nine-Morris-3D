#include <fstream>
#include <string>
#include <exception>

#include <spdlog/spdlog.h>

#include "opengl/debug_opengl.h"
#include "other/logging.h"
#include "other/user_data.h"

#define LOG_FILE "log.txt"

/* SPDLOG_TRACE, SPDLOG_DEBUG, SPDLOG_INFO,
SPDLOG_WARN, SPDLOG_ERROR, SPDLOG_CRITICAL */

namespace logging {
    static std::string path(const char* file) {
#ifndef NDEBUG
        // Use relative path for both operating systems
        return std::string(file);
#else
    #if defined(__GNUG__)
        std::string path = user_data::get_user_data_path() + "/" + file;
        return path;
    #elif defined(_MSC_VER)
        // TODO implement this
        return std::string("");
    #else
        #error "GCC or MSVC must be used (for now)"
    #endif
#endif
    }

    void initialize() {
        spdlog::set_pattern("%^[%l] [thread %t] [%H:%M:%S]%$ %v");
        spdlog::set_level(spdlog::level::trace);
    }

    void log_opengl_and_dependencies_info(LogTarget target) {
        const std::string contents = debug_opengl::get_info();

        if (target == LogTarget::File) {
            std::string file_path;
            try {
                file_path = path(LOG_FILE);
            } catch (const std::runtime_error& e) {
                spdlog::error("{}", e.what());
                return;
            }

            std::ofstream file = std::ofstream(file_path, std::ios::out | std::ios::trunc);
            file << contents.c_str();
            file.close();
        } else if (target == LogTarget::Console) {
            spdlog::info("{}", contents.c_str());
        }
    }
}
