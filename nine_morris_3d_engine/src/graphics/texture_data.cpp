#include "nine_morris_3d_engine/graphics/texture_data.hpp"

#include <cstdlib>
#include <mutex>

#include <stb_image.h>
#include <stb_image_resize2.h>

#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

// Define these to guarantee we're only using malloc
#define STBI_MALLOC(size) std::malloc(size)
#define STBI_REALLOC(ptr, new_size) std::realloc(ptr, new_size)
#define STBI_FREE(ptr) std::free(ptr)

#define STBIR_MALLOC(size, user_data) (static_cast<void>(user_data), std::malloc(size))
#define STBIR_FREE(ptr, user_data) (static_cast<void>(user_data), std::free(ptr))

namespace sm {
    static constexpr int CHANNELS {4};
    static std::mutex g_mutex;

    TextureData::TextureData(const std::string& buffer, const TexturePostProcessing& post_processing) {
        {
            std::lock_guard lock {g_mutex};  // For some stupid reason stbi_load_from_memory is not thread safe :P

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
        }

        if (data == nullptr) {
            SM_THROW_ERROR(ResourceError, "Could not load texture data");
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
