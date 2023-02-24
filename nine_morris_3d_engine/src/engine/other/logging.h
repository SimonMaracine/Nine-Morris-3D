#pragma once

#include <spdlog/spdlog.h>

#include "engine/application/platform.h"

/**
 * Use LOG_ macros for messages to be printed only in debug mode.
 * Use LOG_DIST_ macros for messages to be printed both in debug and release mode.
 *
 * Messages in debug mode are logged in the console. Messages in release mode are logged
 * in a log file (with console as fallback).
 *
 * spdlog is used directly only in the engine, which means it should work fine.
 */

#if defined(NM3D_PLATFORM_RELEASE_DISTRIBUTION)
    #define LOG_DEBUG(...) ((void) 0)
    #define LOG_INFO(...) ((void) 0)
    #define LOG_WARNING(...) ((void) 0)
    #define LOG_ERROR(...) ((void) 0)
    #define LOG_CRITICAL(...) ((void) 0)
#else
    #define LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(logging::get_global_logger(), __VA_ARGS__)
    #define LOG_INFO(...) SPDLOG_LOGGER_INFO(logging::get_global_logger(), __VA_ARGS__)
    #define LOG_WARNING(...) SPDLOG_LOGGER_WARN(logging::get_global_logger(), __VA_ARGS__)
    #define LOG_ERROR(...) SPDLOG_LOGGER_ERROR(logging::get_global_logger(), __VA_ARGS__)
    #define LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(logging::get_global_logger(), __VA_ARGS__)
#endif

#define LOG_DIST_DEBUG(...) SPDLOG_LOGGER_DEBUG(logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_INFO(...) SPDLOG_LOGGER_INFO(logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_WARNING(...) SPDLOG_LOGGER_WARN(logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_ERROR(...) SPDLOG_LOGGER_ERROR(logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(logging::get_global_logger(), __VA_ARGS__)

namespace logging {
    enum class LogTarget {
        None, Console, File
    };

    void initialize_for_applications(std::string_view log_file, std::string_view info_file);
    void log_general_information(LogTarget target);
    spdlog::logger* get_global_logger();
    std::string_view get_info_file();
}
