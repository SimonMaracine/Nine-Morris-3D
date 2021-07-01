#include <spdlog/spdlog.h>

/*
SPDLOG_TRACE, SPDLOG_DEBUG, SPDLOG_INFO,
SPDLOG_WARN, SPDLOG_ERROR, SPDLOG_CRITICAL
*/

namespace logging {
    void init() {
        spdlog::set_pattern("[%l] [%H:%M:%S] %v");
        spdlog::set_level(spdlog::level::trace);
    }
}
