#include <string_view>
#include <string>

#include <stb_image.h>

#include "engine/application_base/panic.hpp"
#include "engine/graphics/texture_data.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"
#include "engine/other/encrypt.hpp"

namespace sm {
    TextureData::TextureData(std::string_view file_path, bool flip)
        : file_path(file_path) {
        LOG_DEBUG("Loading texture data `{}`...", file_path);

        stbi_set_flip_vertically_on_load(static_cast<int>(flip));

        int channels;
        data = stbi_load(file_path.data(), &width, &height, &channels, 4);

        if (data == nullptr) {
            LOG_DIST_CRITICAL("Could not load texture data `{}`", file_path);
            panic();
        }
    }

    TextureData::TextureData(Encrypt::EncryptedFile file_path, bool flip)
        : file_path(file_path) {
        LOG_DEBUG("Loading texture data `{}`...", file_path);

        stbi_set_flip_vertically_on_load(static_cast<int>(flip));

        const auto [buffer, buffer_size] {Encrypt::load_file(file_path)};

        int channels;
        data = stbi_load_from_memory(buffer, buffer_size, &width, &height, &channels, 4);

        if (data == nullptr) {
            LOG_DIST_CRITICAL("Could not load texture data `{}`", file_path);
            panic();
        }
    }

    TextureData::~TextureData() {
        SM_ASSERT(data != nullptr, "No data");

        stbi_image_free(data);

        LOG_DEBUG("Freed texture data `{}`", file_path);
    }

    TextureData::Image TextureData::get_data() {
        Image image;
        image.width = width;
        image.height = height;
        image.pixels = data;

        return image;
    }
}
