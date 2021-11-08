#pragma once

// Include spdlog to have acces to logging
#include <spdlog/spdlog.h>

namespace logging {
    enum class LogTarget {
        None, Console, File
    };

    void initialize();
    void log_opengl_and_dependencies_info(LogTarget target);
}
