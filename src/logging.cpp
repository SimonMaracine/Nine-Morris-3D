#include <fstream>

#include <spdlog/spdlog.h>

#include "debug_opengl.h"

/* SPDLOG_TRACE, SPDLOG_DEBUG, SPDLOG_INFO,
SPDLOG_WARN, SPDLOG_ERROR, SPDLOG_CRITICAL */

const char* LOG_FILE = "opengl_log.txt";

namespace logging {
    void init() {
        spdlog::set_pattern("%^[%l] [%H:%M:%S]%$ %v");
        spdlog::set_level(spdlog::level::trace);
    }

    void log_opengl_info(bool to_file) {
        const std::string contents = debug_opengl::get_info();

        if (to_file) {
            std::ofstream stream(LOG_FILE, std::ofstream::out);
            stream << contents.c_str();
            stream.close();
        } else {
            spdlog::info("{}", contents.c_str());
        }
    }
}
