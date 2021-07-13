#include <fstream>

#include <spdlog/spdlog.h>

#include "other/logging.h"
#include "opengl/debug_opengl.h"

/* SPDLOG_TRACE, SPDLOG_DEBUG, SPDLOG_INFO,
SPDLOG_WARN, SPDLOG_ERROR, SPDLOG_CRITICAL */

const char* LOG_FILE = "opengl_log.txt";

namespace logging {
    void init() {
        spdlog::set_pattern("%^[%l] [%H:%M:%S]%$ %v");
        spdlog::set_level(spdlog::level::trace);
    }

    void log_opengl_info(LogTarget target) {
        const std::string contents = debug_opengl::get_info();

        if (target == LogTarget::File) {
            std::ofstream file = std::ofstream(LOG_FILE, std::ofstream::out);
            file << contents.c_str();
            file.close();
        } else if (target == LogTarget::Console) {
            spdlog::info("{}", contents.c_str());
        }
    }
}
