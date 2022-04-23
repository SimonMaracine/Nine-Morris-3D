#pragma once

#include <string>
#include <string_view>

#include <stb_image.h>

struct GLFWimage;

/**
 * Class representing an image. To be used for icons and cursors.
 * Wrap it inside smart pointers.
 */
class IconImage {
public:
    IconImage(std::string_view file_path);
    ~IconImage();

    GLFWimage get_data() const;

    std::string_view get_file_path() const { return file_path; }
private:
    int width = 0, height = 0, channels = 0;
    stbi_uc* data = nullptr;

    std::string file_path;
};
