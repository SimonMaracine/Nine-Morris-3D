#include <fstream>

#include <spdlog/spdlog.h>

#include "opengl/debug_opengl.h"
#include "other/logging.h"

#define LOG_FILE "log.txt"  // TODO This should be changed

/* SPDLOG_TRACE, SPDLOG_DEBUG, SPDLOG_INFO,
SPDLOG_WARN, SPDLOG_ERROR, SPDLOG_CRITICAL */

namespace logging {
    void init() {
        spdlog::set_pattern("%^[%l] [thread %t] [%H:%M:%S]%$ %v");
        spdlog::set_level(spdlog::level::trace);
    }

    void log_opengl_and_dependencies_info(LogTarget target) {
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
