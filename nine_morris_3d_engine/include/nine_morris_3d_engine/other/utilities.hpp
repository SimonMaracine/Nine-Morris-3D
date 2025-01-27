#pragma once

#include <string>
#include <initializer_list>
#include <iterator>
#include <filesystem>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/other/internal/array.hpp"

namespace sm::utils {
    template<typename T, typename C, C Capacity>
    using Array = internal::Array<T, C, Capacity>;

    struct AABB {
        glm::vec3 min {};
        glm::vec3 max {};
    };

    unsigned int random_int(unsigned int end) noexcept;
    unsigned int random_int(unsigned int begin, unsigned int end) noexcept;

    unsigned short string_to_unsigned_short(const std::string& string);

    void center_image(
        float screen_width,
        float screen_height,
        float image_width,
        float image_height,
        float& x,
        float& y,
        float& width,
        float& height
    ) noexcept;

    std::string file_name(const std::filesystem::path& file_path);
    std::string file_name(const std::string& file_path);

    std::string read_file_ex(const std::filesystem::path& file_path, bool text = false);
    std::string read_file_ex(const std::string& file_path, bool text = false);
    std::string read_file(const std::filesystem::path& file_path, bool text = false);
    std::string read_file(const std::string& file_path, bool text = false);

    void write_file_ex(const std::filesystem::path& file_path, const std::string& buffer, bool text = false);
    void write_file_ex(const std::string& file_path, const std::string& buffer, bool text = false);
    void write_file(const std::filesystem::path& file_path, const std::string& buffer, bool text = false);
    void write_file(const std::string& file_path, const std::string& buffer, bool text = false);

    template<typename T>
    T map(T x, T in_min, T in_max, T out_min, T out_max) noexcept {
        return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
    }

    template<typename T>
    T choice(std::initializer_list<T> list) noexcept {
        const unsigned int index {random_int(static_cast<unsigned int>(list.size() - 1))};

        return list.begin()[index];
    }

    template<typename T, typename Iter>
    T choice(Iter first, Iter last) noexcept {
        const unsigned int index {random_int(static_cast<unsigned int>(std::distance(first, last)))};

        return first[index];
    }
}
