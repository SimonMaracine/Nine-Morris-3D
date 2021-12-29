#pragma once

#include <memory>

// Include spdlog to have acces to logging
#include <spdlog/spdlog.h>

#ifdef NDEBUG
    #define REL_DEBUG(...) logging::release_logger->debug(__VA_ARGS__)
    #define REL_INFO(...) logging::release_logger->info(__VA_ARGS__)
    #define REL_WARN(...) logging::release_logger->warn(__VA_ARGS__)
    #define REL_ERROR(...) logging::release_logger->error(__VA_ARGS__)
    #define REL_CRITICAL(...) logging::release_logger->critical(__VA_ARGS__)
#else
    #define REL_DEBUG(...) spdlog::debug(__VA_ARGS__)
    #define REL_INFO(...) spdlog::info(__VA_ARGS__)
    #define REL_WARN(...) spdlog::warn(__VA_ARGS__)
    #define REL_ERROR(...) spdlog::error(__VA_ARGS__)
    #define REL_CRITICAL(...) spdlog::critical(__VA_ARGS__)
#endif

namespace logging {
    extern std::shared_ptr<spdlog::logger> release_logger;

    enum class LogTarget {
        None, Console, File
    };

    void initialize();
    void log_opengl_and_dependencies_info(LogTarget target);
}
