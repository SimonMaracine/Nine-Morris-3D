#include <string>

#include <stb_image.h>

#include "other/texture_data.h"
#include "other/logging.h"

TextureData::TextureData(const std::string& file_path, bool flip) {
    SPDLOG_DEBUG("Loading texture '{}'...", file_path.c_str());

    stbi_set_flip_vertically_on_load((int) flip);

    data = stbi_load(file_path.c_str(), &width, &height, &channels, 0);

    if (!data) {
        spdlog::critical("Could not load texture '{}'", file_path.c_str());
        std::exit(1);
    }

    name = file_path;
}

TextureData::~TextureData() {
    stbi_image_free(data);

    SPDLOG_DEBUG("Freed texture memory '{}'", name.c_str());
}
