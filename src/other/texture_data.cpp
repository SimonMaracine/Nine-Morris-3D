#include <string_view>

#include <stb_image.h>

#include "other/texture_data.h"
#include "other/logging.h"
#include "other/assert.h"

TextureData::TextureData(std::string_view file_path, bool flip) {
    DEB_DEBUG("Loading texture '{}'...", file_path.data());

    stbi_set_flip_vertically_on_load(static_cast<int>(flip));

    data = stbi_load(file_path.data(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture '{}', exiting...", file_path.data());
        exit(1);
    }

    this->file_path = file_path;
}

TextureData::~TextureData() {
    ASSERT(data != nullptr, "No data");

    stbi_image_free(data);

    DEB_DEBUG("Freed texture memory '{}'", file_path.c_str());
}
