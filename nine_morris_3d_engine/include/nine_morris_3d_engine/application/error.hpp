#pragma once

#include <stdexcept>
#include <string>

#include <spdlog/fmt/fmt.h>

// Can only be called after logging is initialized
#define SM_THROW_ERROR(error, ...) \
    LOG_DIST_CRITICAL(__VA_ARGS__); \
    throw error(fmt::format(__VA_ARGS__))

namespace sm {
    struct RuntimeError : std::runtime_error {
        explicit RuntimeError(const char* message)
            : std::runtime_error(message) {}
        explicit RuntimeError(const std::string& message)
            : std::runtime_error(message) {}
    };

    struct InitializationError : RuntimeError {
        explicit InitializationError(const char* message)
            : RuntimeError(message) {}
        explicit InitializationError(const std::string& message)
            : RuntimeError(message) {}
    };

    struct ResourceError : RuntimeError {
        explicit ResourceError(const char* message)
            : RuntimeError(message) {}
        explicit ResourceError(const std::string& message)
            : RuntimeError(message) {}
    };

    struct OtherError : RuntimeError {
        explicit OtherError(const char* message)
            : RuntimeError(message) {}
        explicit OtherError(const std::string& message)
            : RuntimeError(message) {}
    };

    // Used only by the client code
    struct ApplicationError : RuntimeError {
        explicit ApplicationError(const char* message)
            : RuntimeError(message) {}
        explicit ApplicationError(const std::string& message)
            : RuntimeError(message) {}
    };
}
