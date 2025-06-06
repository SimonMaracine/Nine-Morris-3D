#include "nine_morris_3d_engine/graphics/texture_data.hpp"

#include <cstdlib>
#include <mutex>

#include <stb_image.h>
#include <stb_image_resize2.h>

#include "nine_morris_3d_engine/application/internal/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    static constexpr int CHANNELS {4};
    static std::mutex g_mutex;

    TextureData::TextureData(const std::string& buffer, const TexturePostProcessing& post_processing) {
        {
            std::lock_guard lock {g_mutex};  // For some stupid reason stbi_load_from_memory is not thread safe :P

            stbi_set_flip_vertically_on_load(static_cast<int>(post_processing.flip));

            int channels {};

            m_data = stbi_load_from_memory(
                reinterpret_cast<const unsigned char*>(buffer.data()),
                static_cast<int>(buffer.size()),
                &m_width,
                &m_height,
                &channels,
                CHANNELS
            );
        }

        if (m_data == nullptr) {
            SM_THROW_ERROR(internal::ResourceError, "Could not load texture data");
        }

        if (post_processing.size == TextureSize::Half) {
            resize(m_width / 2, m_height / 2);

            LOG_DEBUG("Resized texture");
        }

        LOG_DEBUG("Loaded texture data");
    }

    TextureData::~TextureData() {
        assert(m_data != nullptr);

        stbi_image_free(m_data);

        LOG_DEBUG("Freed texture data");
    }

    int TextureData::get_width() const {
        return m_width;
    }

    int TextureData::get_height() const {
        return m_height;
    }

    const unsigned char* TextureData::get_data() const {
        return m_data;
    }

    unsigned char* TextureData::get_data() {
        return m_data;
    }

    void TextureData::resize(int new_width, int new_height) {
        unsigned char* new_data {
            stbir_resize_uint8_linear(m_data, m_width, m_height, 0, nullptr, new_width, new_height, 0, STBIR_RGBA)
        };
        stbi_image_free(m_data);

        m_data = new_data;
        m_width = new_width;
        m_height = new_height;
    }
}
