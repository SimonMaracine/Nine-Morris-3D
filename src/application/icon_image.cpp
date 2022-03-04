#include <string>

#include <GLFW/glfw3.h>
#include <stb_image.h>

#include "application/icon_image.h"
#include "other/logging.h"

IconImage::IconImage(const std::string& file_path) {
    stbi_set_flip_vertically_on_load(0);

    data = stbi_load(file_path.c_str(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load icon image '{}', exiting...", file_path.c_str());
        exit(1);
    }

    this->file_path = file_path;

    DEB_INFO("Loaded icon image data '{}'", file_path.c_str());
}

IconImage::~IconImage() {
    stbi_image_free(data);

    DEB_INFO("Freed icon image data '{}'", file_path.c_str());
}

GLFWimage IconImage::get_data() const {
    GLFWimage image;
    image.width = width;
    image.height = height;
    image.pixels = data;

    return image;
}
