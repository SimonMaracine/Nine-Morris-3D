#include <stb_image.h>
#include <cppblowfish/cppblowfish.h>

#include "nine_morris_3d_engine/other/texture_data.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encrypt.h"

TextureData::TextureData(std::string_view file_path, bool flip)
    : file_path(file_path) {
    DEB_DEBUG("Loading texture data `{}`...", file_path);

    stbi_set_flip_vertically_on_load(static_cast<int>(flip));

    int channels;
    data = stbi_load(file_path.data(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture data `{}`, exiting...", file_path);
        exit(1);
    }
}

TextureData::TextureData(encrypt::EncryptedFile file_path, bool flip)
    : file_path(file_path) {
    DEB_DEBUG("Loading texture data `{}`...", file_path);

    const cppblowfish::Buffer buffer = encrypt::load_file(file_path);

    stbi_set_flip_vertically_on_load(static_cast<int>(flip));

    int channels;
    data = stbi_load_from_memory(buffer.get(), buffer.size() - buffer.padding(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture data `{}`, exiting...", file_path);
        exit(1);
    }
}

TextureData::~TextureData() {
    ASSERT(data != nullptr, "No data");

    stbi_image_free(data);

    DEB_DEBUG("Freed texture data `{}`", file_path);
}

Image TextureData::get_data() {
    Image image;
    image.width = width;
    image.height = height;
    image.pixels = data;

    return image;
}
