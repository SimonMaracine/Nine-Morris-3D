#pragma once

#include <string>
#include <memory>

#include <spdlog/spdlog.h>

#include "engine/application_base/platform.hpp"

/*
    Use LOG_ macros for messages to be printed only in debug mode.
    Use LOG_DIST_ macros for messages to be printed both in debug and release mode.

    Messages in debug mode are printed in the console. Messages in release mode are printed
    in a log file (with console as fallback).

    Use LOG_CRITICAL only right before panicking.

    spdlog is used directly only in the engine, which means it should work fine.
*/

#if defined(SM_BUILD_DISTRIBUTION)
    #define LOG_DEBUG(...) static_cast<void>(0)
    #define LOG_INFO(...) static_cast<void>(0)
    #define LOG_WARNING(...) static_cast<void>(0)
    #define LOG_ERROR(...) static_cast<void>(0)
    #define LOG_CRITICAL(...) static_cast<void>(0)
#else
    #define LOG_DEBUG(...) SPDLOG_LOGGER_DEBUG(sm::Logging::get_global_logger(), __VA_ARGS__)
    #define LOG_INFO(...) SPDLOG_LOGGER_INFO(sm::Logging::get_global_logger(), __VA_ARGS__)
    #define LOG_WARNING(...) SPDLOG_LOGGER_WARN(sm::Logging::get_global_logger(), __VA_ARGS__)
    #define LOG_ERROR(...) SPDLOG_LOGGER_ERROR(sm::Logging::get_global_logger(), __VA_ARGS__)
    #define LOG_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(sm::Logging::get_global_logger(), __VA_ARGS__)
#endif

#define LOG_DIST_DEBUG(...) SPDLOG_LOGGER_DEBUG(sm::Logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_INFO(...) SPDLOG_LOGGER_INFO(sm::Logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_WARNING(...) SPDLOG_LOGGER_WARN(sm::Logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_ERROR(...) SPDLOG_LOGGER_ERROR(sm::Logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(sm::Logging::get_global_logger(), __VA_ARGS__)

namespace sm {
    class Logging {  // FIXME
    public:
        static void initialize(const std::string& log_file);
        static void uninitialize();
        static spdlog::logger* get_global_logger();
    private:
        static void set_fallback_logger_distribution(const char* error_message);

        static std::shared_ptr<spdlog::logger> global_logger;  // FIXME
    };
}
