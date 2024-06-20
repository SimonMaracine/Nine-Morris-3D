#pragma once

#include <string>
#include <stdexcept>

namespace sm {
    namespace utils {
        std::string read_file_ex(const std::string& file_path);
        std::string read_file(const std::string& file_path);

        struct FileReadError : std::runtime_error {
            explicit FileReadError(const std::string& message)
                : std::runtime_error(message) {}
            explicit FileReadError(const char* message)
                : std::runtime_error(message) {}
        };
    };
}
