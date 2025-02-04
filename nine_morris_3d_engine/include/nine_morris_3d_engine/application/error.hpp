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
    // Base class for the errors thrown by various systems of the application
    struct RuntimeError : std::runtime_error {
        explicit RuntimeError(const char* message)
            : std::runtime_error(message) {}
        explicit RuntimeError(const std::string& message)
            : std::runtime_error(message) {}

        virtual const char* type() const noexcept = 0;
    };

    // Error related to video (window, OpenGL context etc.)
    struct VideoError : RuntimeError {
        explicit VideoError(const char* message)
            : RuntimeError(message) {}
        explicit VideoError(const std::string& message)
            : RuntimeError(message) {}

        const char* type() const noexcept override {
            return "video";
        }
    };

    // Error related to audio
    struct AudioError : RuntimeError {
        explicit AudioError(const char* message)
            : RuntimeError(message) {}
        explicit AudioError(const std::string& message)
            : RuntimeError(message) {}

        const char* type() const noexcept override {
            return "audio";
        }
    };

    // Error related to application resources
    struct ResourceError : RuntimeError {
        explicit ResourceError(const char* message)
            : RuntimeError(message) {}
        explicit ResourceError(const std::string& message)
            : RuntimeError(message) {}

        const char* type() const noexcept override {
            return "resource";
        }
    };

    // Any other application error
    struct OtherError : RuntimeError {
        explicit OtherError(const char* message)
            : RuntimeError(message) {}
        explicit OtherError(const std::string& message)
            : RuntimeError(message) {}

        const char* type() const noexcept override {
            return "other";
        }
    };

    // Error used only by the client code
    // Not thrown by the engine
    struct ApplicationError : RuntimeError {
        explicit ApplicationError(const char* message)
            : RuntimeError(message) {}
        explicit ApplicationError(const std::string& message)
            : RuntimeError(message) {}

        const char* type() const noexcept override {
            return "application";
        }
    };
}
