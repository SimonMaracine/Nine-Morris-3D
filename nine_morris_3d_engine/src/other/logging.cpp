#include "engine/other/logging.hpp"

#include <fstream>

#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

#include "engine/audio/openal/debug.hpp"
#include "engine/graphics/opengl/debug.hpp"
#include "engine/other/file_system.hpp"
#include "engine/other/dependencies.hpp"

#define LOG_PATTERN_DEVELOPMENT "%^[%l] [%t] [%H:%M:%S]%$ %v"
#define LOG_PATTERN_DISTRIBUTION "%^[%l] [%t] [%!:%#] [%c]%$ %v"

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
    void Logging::initialize_applications([[maybe_unused]] const std::string& log_file, const std::string& info_file) {
        Logging::info_file = info_file;

#ifdef SM_BUILD_DISTRIBUTION
        const std::string file_path {FileSystem::path_for_logs(log_file)};

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

    void Logging::log_general_information(LogTarget target) {
        std::string contents;

        contents += '\n';
        contents += AlDebug::get_information();
        contents += GlDebug::get_information();
        contents += Dependencies::get_information();

        switch (target) {
            case LogTarget::File: {
                const std::string file_path {FileSystem::path_logs(info_file)};

                std::ofstream stream {file_path};

                if (!stream.is_open()) {
                    LOG_DIST_ERROR("Could not open file `{}` for writing", file_path);
                    break;
                }

                stream << contents;

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

    spdlog::logger* Logging::get_global_logger() {
        return global_logger.get();
    }

    const std::string& Logging::get_info_file() {
        return info_file;
    }

    void Logging::set_fallback_logger_distribution(const char* error_message) {
        global_logger = spdlog::stdout_color_mt("Distribution Logger Fallback [Console]");
        global_logger->set_pattern(LOG_PATTERN_DISTRIBUTION);
        global_logger->set_level(spdlog::level::trace);

        global_logger->error("Using fallback distribution logger: {}", error_message);
    }

    std::shared_ptr<spdlog::logger> Logging::global_logger;
    std::string Logging::info_file;
}
