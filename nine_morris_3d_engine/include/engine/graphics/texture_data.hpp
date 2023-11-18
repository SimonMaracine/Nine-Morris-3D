#pragma once

#include <string>

#include "engine/other/encrypt.hpp"

namespace sm {
    class GlTexture;
    class GlTextureCubemap;

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

        Image get_data();
        const std::string& get_file_path() { return file_path; }
    private:
        unsigned char* data {nullptr};
        int width {0};
        int height {0};
        std::string file_path;

        friend class GlTexture;
        friend class GlTextureCubemap;
    };
}
