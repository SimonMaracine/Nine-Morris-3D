#pragma once

#include <string>
#include <initializer_list>
#include <iterator>
#include <filesystem>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/other/internal/array.hpp"

// General utility functions and structures

namespace sm {
    namespace utils {
        template<typename T, typename C, C Capacity>
        using Array = internal::Array<T, C, Capacity>;

        // Axis-aligned bounding box
        struct AABB {
            glm::vec3 min {};
            glm::vec3 max {};
        };

        // Generate a random unsigned integer
        unsigned int random_int(unsigned int end);
        unsigned int random_int(unsigned int begin, unsigned int end);

        // Safely convert a string to unsigned short
        unsigned short string_to_unsigned_short(const std::string& string);

        // Center and fit an image on a screen
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

        // Retrieve an environment variable
        const char* get_environment_variable(const std::string& variable);

        // Retrieve the name of a file
        std::string file_name(const std::filesystem::path& file_path);

        // Read a file into a string buffer
        std::string read_file_ex(const std::filesystem::path& file_path, bool text = false);
        std::string read_file(const std::filesystem::path& file_path, bool text = false);

        // Write a file from a string buffer
        void write_file_ex(const std::filesystem::path& file_path, const std::string& buffer, bool text = false);
        void write_file(const std::filesystem::path& file_path, const std::string& buffer, bool text = false);

        // Map a value from a range to another
        template<typename T>
        T map(T x, T in_min, T in_max, T out_min, T out_max) {
            return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
        }

        // Get a random value from a list
        template<typename T>
        T choice(std::initializer_list<T> list) {
            const auto index {random_int(static_cast<unsigned int>(list.size() - 1))};

            return list.begin()[index];
        }

        // Get a random value from a list
        template<typename T, typename Iter>
        T choice(Iter first, Iter last) {
            const auto index {random_int(static_cast<unsigned int>(std::distance(first, last)))};

            return first[index];
        }

        // Mark unreachable sections of code
        [[noreturn]] inline void unreachable() {
#if defined(_MSC_VER) && !defined(__clang__)
            __assume(false);
#else
            __builtin_unreachable();
#endif
        }
    }
}
