#pragma once

#include <spdlog/spdlog.h>

namespace logging {
    enum class LogTarget {
        None, Console, File
    };

    void init();
    void log_opengl_info(LogTarget target);
}
