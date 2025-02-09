#pragma once

#include <string>

namespace sm {
    enum class TextureSize {
        Default,
        Half
    };

    // Parameters used to optionally transform the image during loading
    struct TexturePostProcessing {
        bool flip {true};
        TextureSize size {TextureSize::Default};
    };

    // Resource representing an image data
    class TextureData {
    public:
        TextureData(const std::string& buffer, const TexturePostProcessing& post_processing = {});
        ~TextureData();

        TextureData(const TextureData&) = delete;
        TextureData& operator=(const TextureData&) = delete;
        TextureData(TextureData&&) = delete;
        TextureData& operator=(TextureData&&) = delete;

        // Retrieve image information
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
