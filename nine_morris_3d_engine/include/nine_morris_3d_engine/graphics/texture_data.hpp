#pragma once

#include <string>

namespace sm {
    enum class TextureSize {
        Default,
        Half
    };

    struct TexturePostProcessing {
        bool flip {true};
        TextureSize size {TextureSize::Default};
    };

    class TextureData {
    public:
        TextureData(const std::string& buffer, const TexturePostProcessing& post_processing = {});
        ~TextureData();

        TextureData(const TextureData&) = delete;
        TextureData& operator=(const TextureData&) = delete;
        TextureData(TextureData&&) = delete;
        TextureData& operator=(TextureData&&) = delete;

        int get_width() const;
        int get_height() const;
        const unsigned char* get_data() const;
        unsigned char* get_data();
    private:
        void resize(int new_width, int new_height);

        int m_width {};
        int m_height {};
        unsigned char* m_data {};
    };
}
