#include <stb_image.h>
#include <cppblowfish/cppblowfish.h>

#include "engine/graphics/texture_data.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"
#include "engine/other/encrypt.h"
#include "engine/application/panic.h"

TextureData::TextureData(std::string_view file_path, bool flip)
    : file_path(file_path) {
    LOG_DEBUG("Loading texture data `{}`...", file_path);

    stbi_set_flip_vertically_on_load(static_cast<int>(flip));

    int channels;
    data = stbi_load(file_path.data(), &width, &height, &channels, 4);

    if (data == nullptr) {
        LOG_DIST_CRITICAL("Could not load texture data `{}`", file_path);
        panic::panic();
    }
}

TextureData::TextureData(Encrypt::EncryptedFile file_path, bool flip)
    : file_path(file_path) {
    LOG_DEBUG("Loading texture data `{}`...", file_path);

    stbi_set_flip_vertically_on_load(static_cast<int>(flip));

    const cppblowfish::Buffer buffer = Encrypt::load_file(file_path);

    int channels;
    data = stbi_load_from_memory(buffer.get(), buffer.size() - buffer.padding(), &width, &height, &channels, 4);

    if (data == nullptr) {
        LOG_DIST_CRITICAL("Could not load texture data `{}`", file_path);
        panic::panic();
    }
}

TextureData::~TextureData() {
    ASSERT(data != nullptr, "No data");

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
