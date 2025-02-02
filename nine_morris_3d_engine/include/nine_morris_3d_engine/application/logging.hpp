#pragma once

#include <spdlog/spdlog.h>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/internal/logging_base.hpp"

// Never include this header file in another header file!

/*
    Use LOG_* macros for messages to be printed only in non-distribution mode.
    Use LOG_DIST_* macros for messages to be printed always.

    Messages in non-distribution mode are printed in the console. In distribution mode they are printed
    in a log file (with console as fallback).

    Use LOG_DIST_CRITICAL only right before aborting or throwing.

    LOG_* macros are not exception-safe, but they may still be used in development.
*/

#ifdef SM_BUILD_DISTRIBUTION
    #define LOG_DEBUG(...) static_cast<void>(0)
    #define LOG_INFO(...) static_cast<void>(0)
    #define LOG_WARNING(...) static_cast<void>(0)
    #define LOG_ERROR(...) static_cast<void>(0)
    #define LOG_CRITICAL(...) static_cast<void>(0)
#else
    #define LOG_DEBUG(...) sm::internal::Logging::get_global_logger()->debug(__VA_ARGS__)
    #define LOG_INFO(...) sm::internal::Logging::get_global_logger()->info(__VA_ARGS__)
    #define LOG_WARNING(...) sm::internal::Logging::get_global_logger()->warn(__VA_ARGS__)
    #define LOG_ERROR(...) sm::internal::Logging::get_global_logger()->error(__VA_ARGS__)
    #define LOG_CRITICAL(...) sm::internal::Logging::get_global_logger()->critical(__VA_ARGS__)
#endif

#define LOG_DIST_DEBUG(...) SPDLOG_LOGGER_DEBUG(sm::internal::Logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_INFO(...) SPDLOG_LOGGER_INFO(sm::internal::Logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_WARNING(...) SPDLOG_LOGGER_WARN(sm::internal::Logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_ERROR(...) SPDLOG_LOGGER_ERROR(sm::internal::Logging::get_global_logger(), __VA_ARGS__)
#define LOG_DIST_CRITICAL(...) SPDLOG_LOGGER_CRITICAL(sm::internal::Logging::get_global_logger(), __VA_ARGS__)
