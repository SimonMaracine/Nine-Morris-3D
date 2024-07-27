#include "nine_morris_3d_engine/application/internal/logging_base.hpp"

#include <fstream>
#include <cstddef>
#include <cstdlib>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "nine_morris_3d_engine/application/platform.hpp"

[[maybe_unused]] static const char* LOG_PATTERN_DEVELOPMENT {"%^[%l] [%t] [%H:%M:%S]%$ %v"};
static const char* LOG_PATTERN_DISTRIBUTION {"%^[%l] [%t] [%!:%#] [%c]%$ %v"};
static constexpr std::size_t FILE_SIZE {1048576 * 1};  // 1 MiB
static constexpr std::size_t ROTATING_FILES {2};  // 3 total log files

namespace sm::internal {
    Logging::Logging([[maybe_unused]] const std::string& log_file, [[maybe_unused]] const FileSystem& fs) {
#ifdef SM_BUILD_DISTRIBUTION
        const std::string file_path {fs.path_logs(log_file)};

        try {
            g_logger = spdlog::rotating_logger_mt("Distribution Logger [File]", file_path, FILE_SIZE, ROTATING_FILES);
        } catch (const spdlog::spdlog_ex& e) {
            set_fallback_logger_distribution(e.what());
            return;
        }

        g_logger->set_pattern(LOG_PATTERN_DISTRIBUTION);
        g_logger->set_level(spdlog::level::trace);
        g_logger->flush_on(spdlog::level::err);
#else
        g_logger = spdlog::stdout_color_mt("Development Logger [Console]");
        g_logger->set_pattern(LOG_PATTERN_DEVELOPMENT);
        g_logger->set_level(spdlog::level::trace);
#endif
    }

    Logging::~Logging() {
        g_logger->flush();
        g_logger.reset();
    }

    spdlog::logger* Logging::get_global_logger() noexcept {
        return g_logger.get();
    }

    void Logging::abort() noexcept {
        std::abort();
    }

    void Logging::set_fallback_logger_distribution(const char* error_message) {
        g_logger = spdlog::stdout_color_mt("Distribution Logger Fallback [Console]");
        g_logger->set_pattern(LOG_PATTERN_DISTRIBUTION);
        g_logger->set_level(spdlog::level::trace);

        g_logger->error("Using fallback distribution logger: {}", error_message);
    }

    std::shared_ptr<spdlog::logger> Logging::g_logger;
}
