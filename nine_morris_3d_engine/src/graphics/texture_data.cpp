#include "engine/graphics/texture_data.hpp"

#include <stb_image.h>

#include "engine/application_base/error.hpp"
#include "engine/application_base/logging.hpp"

namespace sm {
    static constexpr int CHANNELS {4};

    TextureData::TextureData(const std::string& buffer, bool flip) {
        stbi_set_flip_vertically_on_load(static_cast<int>(flip));

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

        LOG_DEBUG("Loaded texture data");
    }

    TextureData::~TextureData() {
        assert(data != nullptr);

        stbi_image_free(data);

        LOG_DEBUG("Freed texture data");
    }
}
