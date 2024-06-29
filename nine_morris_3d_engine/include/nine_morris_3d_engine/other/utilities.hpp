#pragma once

#include <string>
#include <stdexcept>

namespace sm {
    namespace utils {
        template<typename T>
        T map(T x, T in_min, T in_max, T out_min, T out_max) {
            return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }

        void center_image(
            float screen_width,
            float screen_height,
            float image_width,
            float image_height,
            float& x,
            float& y,
            float& width,
            float& height
        );

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
