#pragma once

#include <stdexcept>
#include <string>

namespace networking::internal {
    // Thrown for any generic connection problem
    struct ConnectionError : public std::runtime_error {
        explicit ConnectionError(const std::string& message)
            : std::runtime_error(message) {}
        explicit ConnectionError(const char* message)
            : std::runtime_error(message) {}
    };

    // Thrown for any serialization errors occuring in message
    struct SerializationError : public std::runtime_error {
        explicit SerializationError(const std::string& message)
            : std::runtime_error(message) {}
        explicit SerializationError(const char* message)
            : std::runtime_error(message) {}
    };
}
