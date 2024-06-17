#include "engine/graphics/texture_data.hpp"

#include <stb_image.h>

#include "engine/application_base/panic.hpp"
#include "engine/other/logging.hpp"

namespace sm {
    static constexpr int CHANNELS {4};

    TextureData::TextureData(const std::string& file_path, bool flip)
        : file_path(file_path) {
        LOG_DEBUG("Loading texture data `{}`...", file_path);

        stbi_set_flip_vertically_on_load(static_cast<int>(flip));

        int channels {};
        data = stbi_load(file_path.c_str(), &width, &height, &channels, CHANNELS);

        if (data == nullptr) {
            LOG_DIST_CRITICAL("Could not load texture data `{}`", file_path);
            throw ResourceLoadingError;
        }
    }

    // TextureData::TextureData(const EncrFile& file_path, bool flip)
    //     : file_path(file_path) {
    //     LOG_DEBUG("Loading texture data `{}`...", file_path);

    //     stbi_set_flip_vertically_on_load(static_cast<int>(flip));

    //     const auto [buffer, buffer_size] {Encrypt::load_file(file_path)};

    //     int channels;
    //     data = stbi_load_from_memory(buffer, buffer_size, &width, &height, &channels, CHANNELS);

    //     if (data == nullptr) {
    //         LOG_DIST_CRITICAL("Could not load texture data `{}`", file_path);
    //         throw ResourceLoadingError;
    //     }
    // }

    TextureData::~TextureData() {
        assert(data != nullptr);

        stbi_image_free(data);

        LOG_DEBUG("Freed texture data `{}`", file_path);
    }
}
