#include "nine_morris_3d_engine/application/logging.hpp"

#include <fstream>
#include <cstddef>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "nine_morris_3d_engine/application/file_system.hpp"

static const char* LOG_PATTERN_DEVELOPMENT {"%^[%l] [%t] [%H:%M:%S]%$ %v"};
static const char* LOG_PATTERN_DISTRIBUTION {"%^[%l] [%t] [%!:%#] [%c]%$ %v"};
static constexpr std::size_t FILE_SIZE {1048576 * 2};  // 2 MiB
static constexpr std::size_t ROTATING_FILES {2};  // 3 total log files

/*
    SPDLOG_TRACE
    SPDLOG_DEBUG
    SPDLOG_INFO
    SPDLOG_WARN
    SPDLOG_ERROR
    SPDLOG_CRITICAL
*/

namespace sm {
    spdlog::logger* Logging::get_global_logger() {
        return global_logger.get();
    }

    Logging::Logging([[maybe_unused]] const std::string& log_file, [[maybe_unused]] const FileSystem& fs) {
#ifdef SM_BUILD_DISTRIBUTION
        const std::string file_path {fs.path_for_logs(log_file)};

        try {
            global_logger = spdlog::rotating_logger_mt("Distribution Logger [File]", file_path, FILE_SIZE, ROTATING_FILES);
        } catch (const spdlog::spdlog_ex& e) {
            set_fallback_logger_distribution(e.what());
            return;
        }

        global_logger->set_pattern(LOG_PATTERN_DISTRIBUTION);
        global_logger->set_level(spdlog::level::trace);
        global_logger->flush_on(spdlog::level::info);
#else
        global_logger = spdlog::stdout_color_mt("Development Logger [Console]");
        global_logger->set_pattern(LOG_PATTERN_DEVELOPMENT);
        global_logger->set_level(spdlog::level::trace);
#endif
    }

    void Logging::set_fallback_logger_distribution(const char* error_message) {
        global_logger = spdlog::stdout_color_mt("Distribution Logger Fallback [Console]");
        global_logger->set_pattern(LOG_PATTERN_DISTRIBUTION);
        global_logger->set_level(spdlog::level::trace);

        global_logger->error("Using fallback distribution logger: {}", error_message);
    }

    std::shared_ptr<spdlog::logger> Logging::global_logger;
}
