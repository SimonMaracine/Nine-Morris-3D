#pragma once

#include <string>

namespace sm {
    enum class Size {
        Default,
        Half
    };

    struct TexturePostProcessing {
        bool flip {true};
        Size size {Size::Default};
    };

    class TextureData {
    public:
        TextureData(const std::string& buffer, const TexturePostProcessing& post_processing = {});
        ~TextureData();

        TextureData(const TextureData&) = delete;
        TextureData& operator=(const TextureData&) = delete;
        TextureData(TextureData&&) = delete;
        TextureData& operator=(TextureData&&) = delete;

        int get_width() const { return width; }
        int get_height() const { return height; }
        const unsigned char* get_data() const { return data; }
    private:
        void resize(int new_width, int new_height);

        int width {};
        int height {};
        unsigned char* data {nullptr};
    };
}
