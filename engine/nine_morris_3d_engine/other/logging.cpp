#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/graphics/debug_opengl.h"
#include "nine_morris_3d_engine/other/path.h"
#include "nine_morris_3d_engine/other/user_data.h"
#include "nine_morris_3d_engine/other/logging.h"

#define LOG_PATTERN_DEBUG "%^[%l] [th %t] [%H:%M:%S]%$ %v"
#define LOG_PATTERN_RELEASE "%^[%l] [th %t] [%!:%#] [%c]%$ %v"

#define FILE_SIZE 1048576 * 2  // 2 MiB
#define ROTATING_FILES 2  // 3 total log files

/*
    SPDLOG_TRACE,
    SPDLOG_DEBUG,
    SPDLOG_INFO,
    SPDLOG_WARN,
    SPDLOG_ERROR,
    SPDLOG_CRITICAL
*/

// Should be cleaned up at exit()
static std::shared_ptr<spdlog::logger> _global_logger;

#if defined(PLATFORM_GAME_RELEASE)
static void set_fallback_logger_release(const char* error_message) {
    _global_logger = spdlog::stdout_color_mt("Release Logger Fallback [Console]");
    _global_logger->set_pattern(LOG_PATTERN_RELEASE);
    _global_logger->set_level(spdlog::level::trace);

    _global_logger->error("Using Fallback Logger (Console): {}", error_message);
}
#endif

namespace logging {
    void initialize_for_applications(std::string_view log_file) {
#if defined(PLATFORM_GAME_DEBUG)
        _global_logger = spdlog::stdout_color_mt("Debug Logger [Console]");
        _global_logger->set_pattern(LOG_PATTERN_DEBUG);
        _global_logger->set_level(spdlog::level::trace);

        static_cast<void>(log_file);  // Trick the compiler that we do use log_file
#elif defined(PLATFORM_GAME_RELEASE)
        std::string file_path;

        try {
            file_path = path::path_for_logs(log_file);
        } catch (const user_data::UserNameError& e) {
            set_fallback_logger_release(e.what());
            return;
        }

        try {
            _global_logger = spdlog::rotating_logger_mt(
                "Release Logger [File]", file_path, FILE_SIZE, ROTATING_FILES
            );
        } catch (const spdlog::spdlog_ex& e) {
            set_fallback_logger_release(e.what());
            return;
        }

        _global_logger->set_pattern(LOG_PATTERN_RELEASE);
        _global_logger->set_level(spdlog::level::trace);
        _global_logger->flush_on(spdlog::level::info);
#endif
    }

    void log_opengl_and_dependencies_info(LogTarget target, std::string_view info_file) {
        const std::string contents = debug_opengl::get_info();

        switch (target) {
            case LogTarget::File: {
                std::string file_path;
                try {
                    file_path = path::path_for_logs(info_file);
                } catch (const user_data::UserNameError& e) {
                    REL_ERROR("Could not create info file `{}`: {}", info_file, e.what());
                    break;
                }

                std::ofstream file {file_path, std::ios::trunc};

                if (!file.is_open()) {
                    REL_ERROR("Could not open file `{}` for writing", file_path);
                    break;
                }

                file << contents;

                break;
            }
            case LogTarget::Console:
                REL_INFO("{}", contents);
                break;
            case LogTarget::None:
                break;  // Do nothing
        }
    }

    spdlog::logger* get_global_logger() {
        return _global_logger.get();
    }
}
