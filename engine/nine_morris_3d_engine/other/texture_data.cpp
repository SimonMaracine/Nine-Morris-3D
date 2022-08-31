#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <cppblowfish.h>

#include "nine_morris_3d_engine/other/texture_data.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encryption.h"

TextureData::TextureData(std::string_view file_path, bool flip) {
    DEB_DEBUG("Loading texture data '{}'...", file_path);

    stbi_set_flip_vertically_on_load(static_cast<int>(flip));

    data = stbi_load(file_path.data(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture data '{}', exiting...", file_path);
        exit(1);
    }

    this->file_path = file_path;
}

TextureData::TextureData(encryption::EncryptedFile file_path, bool flip) {
    DEB_DEBUG("Loading texture data '{}'...", file_path);

    cppblowfish::Buffer buffer = encryption::load_file(file_path);

    stbi_set_flip_vertically_on_load(static_cast<int>(flip));

    data = stbi_load_from_memory(buffer.get(), buffer.size() - buffer.padding(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture data '{}', exiting...", file_path);
        exit(1);
    }

    this->file_path = file_path;
}

TextureData::~TextureData() {
    ASSERT(data != nullptr, "No data");

    stbi_image_free(data);

    DEB_DEBUG("Freed texture data '{}'", file_path);
}

GLFWimage TextureData::get_data_glfw() const {
    GLFWimage image;
    image.width = width;
    image.height = height;
    image.pixels = data;

    return image;
}
