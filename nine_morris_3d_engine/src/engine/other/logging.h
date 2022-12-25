#pragma once

#include <spdlog/spdlog.h>

#include "engine/application/platform.h"

/**
 * Use DEB_ macros for messages to be printed only in debug mode.
 * Use REL_ macros for messages to be printed both in debug and release mode.
 *
 * Messages in debug mode are logged in the console. Messages in release mode are logged in a log file (with console as fallback).
 *
 * spdlog is used directly only in the engine, which means it should work fine.
 */

#if defined(NM3D_PLATFORM_RELEASE)
    #define DEB_DEBUG(...) ((void) 0)
    #define DEB_INFO(...) ((void) 0)
    #define DEB_WARNING(...) ((void) 0)
    #define DEB_ERROR(...) ((void) 0)
    #define DEB_CRITICAL(...) ((void) 0)
#elif defined(NM3D_PLATFORM_DEBUG)
    #define DEB_DEBUG(...) SPDLOG_LOGGER_DEBUG(logging::get_global_logger(), __VA_ARGS__)
    #define DEB_INFO(...) SPDLOG_LOGGER_INFO(logging::get_global_logger(), __VA_ARGS__)
    #define DEB_WARNING(...) SPDLOG_LOGGER_WARN(logging::get_global_logger(), __VA_ARGS__)
    #define DEB_ERROR(...) SPDLOG_LOGGER_ERROR(logging::get_global_logger(), __VA_ARGS__)
    #define DEB_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(logging::get_global_logger(), __VA_ARGS__)
#endif

#define REL_DEBUG(...) SPDLOG_LOGGER_DEBUG(logging::get_global_logger(), __VA_ARGS__)
#define REL_INFO(...) SPDLOG_LOGGER_INFO(logging::get_global_logger(), __VA_ARGS__)
#define REL_WARNING(...) SPDLOG_LOGGER_WARN(logging::get_global_logger(), __VA_ARGS__)
#define REL_ERROR(...) SPDLOG_LOGGER_ERROR(logging::get_global_logger(), __VA_ARGS__)
#define REL_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(logging::get_global_logger(), __VA_ARGS__)

namespace logging {
    enum class LogTarget {
        None, Console, File
    };

    void initialize_for_applications(std::string_view log_file);
    void log_opengl_and_dependencies_info(LogTarget target, std::string_view info_file);
    spdlog::logger* get_global_logger();
}
