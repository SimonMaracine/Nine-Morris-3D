#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/graphics/debug_opengl.h"
#include "nine_morris_3d_engine/other/paths.h"
#include "nine_morris_3d_engine/other/user_data.h"
#include "nine_morris_3d_engine/other/logging.h"

#define LOG_PATTERN_DEBUG "%^[%l] [th %t] [%H:%M:%S]%$ %v"
#define LOG_PATTERN_RELEASE "%^[%l] [th %t] [%!:%#] [%c]%$ %v"

/* SPDLOG_TRACE, SPDLOG_DEBUG, SPDLOG_INFO,
SPDLOG_WARN, SPDLOG_ERROR, SPDLOG_CRITICAL */

namespace logging {
    static std::shared_ptr<spdlog::logger> global_logger;

    void initialize(std::string_view log_file) {
#if defined(PLATFORM_GAME_DEBUG)
        global_logger = spdlog::stdout_color_mt("Debug Logger [Console]");
        global_logger->set_pattern(LOG_PATTERN_DEBUG);
        global_logger->set_level(spdlog::level::trace);

        spdlog::set_default_logger(global_logger);

        static_cast<void>(log_file);  // Trick the compiler that we do use log_file
#elif defined(PLATFORM_GAME_RELEASE)
        std::string file_path;
        try {
            file_path = paths::path_for_logs(log_file);
        } catch (const user_data::UserNameError& e) {
            global_logger = spdlog::stdout_color_mt("Release Logger Fallback [Console]");
            global_logger->set_pattern(LOG_PATTERN_RELEASE);
            global_logger->set_level(spdlog::level::trace);

            spdlog::error("Using fallback logger (console)");

            spdlog::set_default_logger(global_logger);
            return;
        }

        try {
            global_logger = spdlog::basic_logger_mt("Release Logger [File]", file_path, false);
        } catch (const spdlog::spdlog_ex& e) {
            global_logger = spdlog::stdout_color_mt("Release Logger Fallback [Console]");
            global_logger->set_pattern(LOG_PATTERN_RELEASE);
            global_logger->set_level(spdlog::level::trace);

            spdlog::error("Using fallback logger (console): {}", e.what());

            spdlog::set_default_logger(global_logger);
            return;
        }

        global_logger->set_pattern(LOG_PATTERN_RELEASE);
        global_logger->set_level(spdlog::level::trace);
        global_logger->flush_on(spdlog::level::info);

        spdlog::set_default_logger(global_logger);
#endif
    }

    void log_opengl_and_dependencies_info(LogTarget target, std::string_view info_file) {
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

                std::ofstream file {file_path, std::ios::trunc};

                if (!file.is_open()) {
                    REL_ERROR("Could not open file '{}' for writing", file_path);
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
}
