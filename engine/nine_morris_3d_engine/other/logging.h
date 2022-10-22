#pragma once

#include <spdlog/spdlog.h>

#include "nine_morris_3d_engine/application/platform.h"

/**
 * Use DEB_ macros for messages to be printed only in debug mode.
 * Use REL_ macros for messages to be printed both in debug and release mode.
 *
 * Messages in debug mode are logged in the console. Messages in release mode are logged in a log file (with console as fallback).
 */

#if defined(PLATFORM_GAME_RELEASE)
    #define DEB_DEBUG(...) ((void) 0)
    #define DEB_INFO(...) ((void) 0)
    #define DEB_WARN(...) ((void) 0)
    #define DEB_ERROR(...) ((void) 0)
    #define DEB_CRITICAL(...) ((void) 0)
#elif defined(PLATFORM_GAME_DEBUG)
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

    void initialize_for_application(std::string_view log_file);
    void log_opengl_and_dependencies_info(LogTarget target, std::string_view info_file);
}
