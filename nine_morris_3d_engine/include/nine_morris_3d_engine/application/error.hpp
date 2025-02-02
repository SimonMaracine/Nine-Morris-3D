#pragma once

#include <stdexcept>
#include <string>

#include <spdlog/fmt/fmt.h>

// Thrown an error and log a critical message
// Used when the engine should panic
// Can only be called after logging is initialized
#define SM_THROW_ERROR(error, ...) \
    LOG_DIST_CRITICAL(__VA_ARGS__); \
    throw error(fmt::format(__VA_ARGS__))

namespace sm {
    // Base class error thrown by various systems of the application
    struct RuntimeError : std::runtime_error {
        explicit RuntimeError(const char* message)
            : std::runtime_error(message) {}
        explicit RuntimeError(const std::string& message)
            : std::runtime_error(message) {}
    };

    // Error related to application initialization
    struct InitializationError : RuntimeError {
        explicit InitializationError(const char* message)
            : RuntimeError(message) {}
        explicit InitializationError(const std::string& message)
            : RuntimeError(message) {}
    };

    // Error related to application resources
    struct ResourceError : RuntimeError {
        explicit ResourceError(const char* message)
            : RuntimeError(message) {}
        explicit ResourceError(const std::string& message)
            : RuntimeError(message) {}
    };

    // Any other application error
    struct OtherError : RuntimeError {
        explicit OtherError(const char* message)
            : RuntimeError(message) {}
        explicit OtherError(const std::string& message)
            : RuntimeError(message) {}
    };

    // Error used only by the client code
    // Not thrown by the engine
    struct ApplicationError : RuntimeError {
        explicit ApplicationError(const char* message)
            : RuntimeError(message) {}
        explicit ApplicationError(const std::string& message)
            : RuntimeError(message) {}
    };
}
