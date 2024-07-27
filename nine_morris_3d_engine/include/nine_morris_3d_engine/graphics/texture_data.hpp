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
        ~TextureData() noexcept;

        TextureData(const TextureData&) = delete;
        TextureData& operator=(const TextureData&) = delete;
        TextureData(TextureData&&) = delete;
        TextureData& operator=(TextureData&&) = delete;

        int get_width() const noexcept;
        int get_height() const noexcept;
        const unsigned char* get_data() const noexcept;
    private:
        void resize(int new_width, int new_height);

        int width {};
        int height {};
        unsigned char* data {};
    };
}
