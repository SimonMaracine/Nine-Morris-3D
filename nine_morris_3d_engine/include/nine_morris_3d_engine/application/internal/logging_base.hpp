#pragma once

#include <string>
#include <memory>

#include <spdlog/spdlog.h>

#include "nine_morris_3d_engine/application/internal/file_system.hpp"

// Separate logging base from logging macros to keep things clean in the engine

namespace sm::internal {
    // Keep logging initialization as a class in order to execute it in the context constructor
    class Logging {
    public:
        Logging(const std::string& log_file, const FileSystem& fs);
        ~Logging();

        // Retrieve the global logger, which is available as long as an instance of this class is alive
        static spdlog::logger* get_global_logger();
    private:
        static void set_fallback_logger_distribution(const char* error_message);

        static std::shared_ptr<spdlog::logger> g_logger;
    };
}
