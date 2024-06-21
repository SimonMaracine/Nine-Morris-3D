#include "engine/graphics/texture_data.hpp"

#include <cstdlib>

#include <stb_image.h>
#include <stb_image_resize2.h>

#include "engine/application_base/error.hpp"
#include "engine/application_base/logging.hpp"

// Define these to guarantee we're only using only malloc
#define STBI_MALLOC(size) std::malloc(size)
#define STBI_REALLOC(ptr, new_size) std::realloc(ptr, new_size)
#define STBI_FREE(ptr) std::free(ptr)

#define STBIR_MALLOC(size, user_data) (static_cast<void>(user_data), std::malloc(size))
#define STBIR_FREE(ptr, user_data) (static_cast<void>(user_data), std::free(ptr))

namespace sm {
    static constexpr int CHANNELS {4};

    TextureData::TextureData(const std::string& buffer, const TexturePostProcessing& post_processing) {
        stbi_set_flip_vertically_on_load(static_cast<int>(post_processing.flip));

        int channels {};

        data = stbi_load_from_memory(
            reinterpret_cast<const unsigned char*>(buffer.data()),
            static_cast<int>(buffer.size()),
            &width,
            &height,
            &channels,
            CHANNELS
        );

        if (data == nullptr) {
            SM_CRITICAL_ERROR(RuntimeError::ResourceLoading, "Could not load texture data");
        }

        if (post_processing.size == Size::Half) {
            resize(width / 2, height / 2);

            LOG_DEBUG("Resized texture");
        }

        LOG_DEBUG("Loaded texture data");
    }

    TextureData::~TextureData() {
        assert(data != nullptr);

        stbi_image_free(data);

        LOG_DEBUG("Freed texture data");
    }

    void TextureData::resize(int new_width, int new_height) {
        unsigned char* new_data {stbir_resize_uint8_linear(data, width, height, 0, nullptr, new_width, new_height, 0, STBIR_RGBA)};
        stbi_image_free(data);

        data = new_data;
        width = new_width;
        height = new_height;
    }
}
