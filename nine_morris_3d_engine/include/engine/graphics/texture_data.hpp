#pragma once

#include <string_view>
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

        TextureData(std::string_view file_path, bool flip = false);
        TextureData(Encrypt::EncryptedFile file_path, bool flip = false);
        ~TextureData();

        TextureData(const TextureData&) = delete;
        TextureData& operator=(const TextureData&) = delete;
        TextureData(TextureData&&) = delete;
        TextureData& operator=(TextureData&&) = delete;

        Image get_data();
        std::string_view get_file_path() { return file_path; }
    private:
        unsigned char* data = nullptr;
        int width = 0;
        int height = 0;
        std::string file_path;

        friend class GlTexture;
        friend class GlTextureCubemap;
    };
}
