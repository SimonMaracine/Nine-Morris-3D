#pragma once

#include <spdlog/spdlog.h>

namespace logging {
    void init();
    void log_opengl_info(bool to_file);
}
