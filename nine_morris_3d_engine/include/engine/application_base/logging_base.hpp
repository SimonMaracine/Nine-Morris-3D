#pragma once

#include <string>
#include <memory>

#include <spdlog/spdlog.h>

// Separate logging base from logging macros to keep things clean in the engine

namespace sm {
    class FileSystem;
    class Ctx;

    class Logging {
    public:
        static spdlog::logger* get_global_logger();
    private:
        Logging(const std::string& log_file, const FileSystem& fs);

        static void set_fallback_logger_distribution(const char* error_message);

        static std::shared_ptr<spdlog::logger> global_logger;

        friend class Ctx;
    };
}
