#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "application/platform.h"
#include "graphics/debug_opengl.h"
#include "nine_morris_3d/paths.h"
#include "other/user_data.h"
#include "other/logging.h"

#define LOG_PATTERN_DEBUG "%^[%l] [th %t] [%H:%M:%S]%$ %v"
#define LOG_PATTERN_RELEASE "%^[%l] [th %t] [%!:%#] [%c]%$ %v"

/* SPDLOG_TRACE, SPDLOG_DEBUG, SPDLOG_INFO,
SPDLOG_WARN, SPDLOG_ERROR, SPDLOG_CRITICAL */

namespace logging {
    static std::shared_ptr<spdlog::logger> release_logger;
    static std::shared_ptr<spdlog::logger> debug_logger;

    static void set_global_logger() {
#if defined(NINE_MORRIS_3D_DEBUG)
        spdlog::set_default_logger(debug_logger);
#elif defined(NINE_MORRIS_3D_RELEASE)
        spdlog::set_default_logger(release_logger);
#endif
    }

    void initialize(const char* log_file) {
        // Initialize debug logger
        debug_logger = spdlog::stdout_color_mt("Debug Logger [Console]");
        debug_logger->set_pattern(LOG_PATTERN_DEBUG);
        debug_logger->set_level(spdlog::level::trace);

        std::string file_path;
        try {
            file_path = paths::path_for_logs(log_file);
        } catch (const user_data::UserNameError& e) {            
            release_logger = spdlog::stdout_color_mt("Release Logger Fallback [Console]");
            release_logger->set_pattern(LOG_PATTERN_RELEASE);
            release_logger->set_level(spdlog::level::trace);

            spdlog::error("Using fallback logger (console)");

            set_global_logger();
            return;
        }

        // Initialize release logger
        try {
            release_logger = spdlog::basic_logger_mt("Release Logger [File]", file_path, false);
        } catch (const spdlog::spdlog_ex& e) {
            release_logger = spdlog::stdout_color_mt("Release Logger Fallback [Console]");
            release_logger->set_pattern(LOG_PATTERN_RELEASE);
            release_logger->set_level(spdlog::level::trace);

            spdlog::error("Using fallback logger (console): {}", e.what());

            set_global_logger();
            return;
        }

        release_logger->set_pattern(LOG_PATTERN_RELEASE);
        release_logger->set_level(spdlog::level::trace);
        release_logger->flush_on(spdlog::level::info);

        set_global_logger();
    }

    void log_opengl_and_dependencies_info(LogTarget target, const char* info_file) {
        const std::string contents = debug_opengl::get_info();

        switch (target) {
            case LogTarget::File: {
                std::string file_path;
                try {
                    file_path = paths::path_for_logs(info_file);
                } catch (const user_data::UserNameError& e) {
                    REL_ERROR("{}", e.what());
                    REL_ERROR("Could not create info file");
                    break;
                }

                std::ofstream file (file_path, std::ios::trunc);

                if (!file.is_open()) {
                    REL_ERROR("Could not open file '{}' for writing", file_path);
                    break;
                }

                file << contents.c_str();

                break;
            }
            case LogTarget::Console:
                REL_INFO("{}", contents);

                break;
            case LogTarget::None:
                // Do nothing
                break;
        }
    }
}
