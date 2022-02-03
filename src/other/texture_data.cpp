#include <string>
#include <cassert>

#include <stb_image.h>

#include "other/texture_data.h"
#include "other/logging.h"

TextureData::TextureData(const std::string& file_path, bool flip) {
    DEB_DEBUG("Loading texture '{}'...", file_path.c_str());

    stbi_set_flip_vertically_on_load(static_cast<int>(flip));

    data = stbi_load(file_path.c_str(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture '{}'", file_path.c_str());
        std::exit(1);
    }

    this->file_path = file_path;
}

TextureData::~TextureData() {
    assert(data != nullptr);

    stbi_image_free(data);

    DEB_DEBUG("Freed texture memory '{}'", file_path.c_str());
}
