#pragma once

#include <string>

#include <stb_image.h>

struct GLFWimage;

class IconImage {
public:
    IconImage(const std::string& file_path);
    ~IconImage();

    GLFWimage get_data() const;
private:
    int width = 0, height = 0, channels = 0;
    stbi_uc* data = nullptr;

    std::string file_path;
};
