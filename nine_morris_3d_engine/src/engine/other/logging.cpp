#include <spdlog/spdlog.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "engine/application_base/platform.h"
#include "engine/audio/openal/info_and_debug.h"
#include "engine/graphics/opengl/info_and_debug.h"
#include "engine/other/logging.h"
#include "engine/other/file_system.h"
#include "engine/other/dependencies.h"

#define LOG_PATTERN_DEBUG "%^[%l] [t %t] [%H:%M:%S]%$ %v"
#define LOG_PATTERN_RELEASE "%^[%l] [t %t] [%!:%#] [%c]%$ %v"

#define FILE_SIZE (1048576 * 2)  // 2 MiB
#define ROTATING_FILES 2  // 3 total log files

/*
    SPDLOG_TRACE
    SPDLOG_DEBUG
    SPDLOG_INFO
    SPDLOG_WARN
    SPDLOG_ERROR
    SPDLOG_CRITICAL
*/

namespace sm {
    // These don't need to be reset explicitly
    // Should be cleaned up at exit()
    static std::shared_ptr<spdlog::logger> global_logger;
    static std::string g_info_file;

#ifdef NM3D_PLATFORM_DISTRIBUTION
    static void set_fallback_logger_release(const char* error_message) {
        global_logger = spdlog::stdout_color_mt("Release Logger Fallback [Console]");
        global_logger->set_pattern(LOG_PATTERN_RELEASE);
        global_logger->set_level(spdlog::level::trace);

        global_logger->error("Using Fallback Logger (Console): {}", error_message);
    }
#endif

    namespace logging {
        void initialize_for_applications([[maybe_unused]] std::string_view log_file, std::string_view info_file) {
            g_info_file = info_file;

#ifdef NM3D_PLATFORM_DISTRIBUTION
            const std::string file_path = file_system::path_for_logs(log_file);

            try {
                global_logger = spdlog::rotating_logger_mt(
                    "Release Logger [File]", file_path, FILE_SIZE, ROTATING_FILES
                );
            } catch (const spdlog::spdlog_ex& e) {
                set_fallback_logger_release(e.what());
                return;
            }

            global_logger->set_pattern(LOG_PATTERN_RELEASE);
            global_logger->set_level(spdlog::level::trace);
            global_logger->flush_on(spdlog::level::info);
#else
            global_logger = spdlog::stdout_color_mt("Debug Logger [Console]");
            global_logger->set_pattern(LOG_PATTERN_DEBUG);
            global_logger->set_level(spdlog::level::trace);
#endif
        }

        void log_general_information(LogTarget target) {
            std::string contents;
            contents.reserve(1024 + 64 + 512);

            contents += gl::get_info();
            contents += al::get_info();
            contents += dependencies::get_info();

            switch (target) {
                case LogTarget::File: {
                    const std::string file_path = file_system::path_for_logs(g_info_file);

                    std::ofstream file {file_path, std::ios::trunc};

                    if (!file.is_open()) {
                        LOG_DIST_ERROR("Could not open file `{}` for writing", file_path);
                        break;
                    }

                    file << contents;

                    break;
                }
                case LogTarget::Console:
                    LOG_DIST_INFO("{}", contents);

                    break;
                case LogTarget::None:
                    // Do nothing

                    break;
            }
        }

        spdlog::logger* get_global_logger() {
            return global_logger.get();
        }

        std::string_view get_info_file() {
            return g_info_file;
        }
    }
}
