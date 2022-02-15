#pragma once

#include <memory>

#include <spdlog/spdlog.h>

#include "application/platform.h"

#if defined(NINE_MORRIS_3D_RELEASE)
    #define DEB_DEBUG(...) ((void) 0)
    #define DEB_INFO(...) ((void) 0)
    #define DEB_WARN(...) ((void) 0)
    #define DEB_ERROR(...) ((void) 0)
    #define DEB_CRITICAL(...) ((void) 0)
#elif defined(NINE_MORRIS_3D_DEBUG)
    #define DEB_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
    #define DEB_INFO(...) SPDLOG_INFO(__VA_ARGS__)
    #define DEB_WARN(...) SPDLOG_WARN(__VA_ARGS__)
    #define DEB_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
    #define DEB_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
#endif

#define REL_DEBUG(...) SPDLOG_DEBUG(__VA_ARGS__)
#define REL_INFO(...) SPDLOG_INFO(__VA_ARGS__)
#define REL_WARN(...) SPDLOG_WARN(__VA_ARGS__)
#define REL_ERROR(...) SPDLOG_ERROR(__VA_ARGS__)
#define REL_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)

namespace logging {
    enum class LogTarget {
        None, Console, File
    };

    void initialize();
    void log_opengl_and_dependencies_info(LogTarget target);
}
