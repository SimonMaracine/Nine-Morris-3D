#pragma once

#include <string>

namespace sm {
    class TextureData {
    public:
        TextureData(const std::string& file_path, bool flip = false);
        ~TextureData();

        TextureData(const TextureData&) = delete;
        TextureData& operator=(const TextureData&) = delete;
        TextureData(TextureData&&) = delete;
        TextureData& operator=(TextureData&&) = delete;

        int get_width() const { return width; }
        int get_height() const { return height; }
        const unsigned char* get_data() const { return data; }
        unsigned char* get_data() { return data; }
        const std::string& get_file_path() const { return file_path; }
    private:
        int width {0};
        int height {0};
        unsigned char* data {nullptr};
        std::string file_path;
    };
}
