#pragma once

#include <string>

#include "engine/other/encrypt.hpp"

namespace sm {
    class TextureData {
    public:
        struct Image {
            int width;
            int height;
            unsigned char* pixels;
        };

        TextureData(const std::string& file_path, bool flip = false);
        TextureData(const EncrFile& file_path, bool flip = false);
        ~TextureData();

        TextureData(const TextureData&) = delete;
        TextureData& operator=(const TextureData&) = delete;
        TextureData(TextureData&&) = delete;
        TextureData& operator=(TextureData&&) = delete;

        Image get_image() const;
        const std::string& get_file_path() const { return file_path; }

        const unsigned char* get_data() const { return data; }
        int get_width() const { return width; }
        int get_height() const { return height; }
    private:
        unsigned char* data {nullptr};
        int width {0};
        int height {0};
        std::string file_path;
    };
}
