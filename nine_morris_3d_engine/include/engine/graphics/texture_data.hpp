#pragma once

#include <string>

namespace sm {
    class TextureData {
    public:
        TextureData(const std::string& buffer, bool flip = false);
        ~TextureData();

        TextureData(const TextureData&) = delete;
        TextureData& operator=(const TextureData&) = delete;
        TextureData(TextureData&&) = delete;
        TextureData& operator=(TextureData&&) = delete;

        int get_width() const { return width; }
        int get_height() const { return height; }
        const unsigned char* get_data() const { return data; }
    private:
        int width {};
        int height {};
        unsigned char* data {nullptr};
    };
}
