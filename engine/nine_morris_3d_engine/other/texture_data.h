#pragma once

#include <stb_image.h>

#include "nine_morris_3d_engine/other/encrypt.h"

// Image struct fully convertible to GLFW's GLFWimage
struct Image {
	int width;
    int height;
    unsigned char* pixels;
};

class TextureData {
public:
	TextureData(std::string_view file_path, bool flip = false);
	TextureData(encrypt::EncryptedFile file_path, bool flip = false);
	~TextureData();

	Image get_data();
	std::string_view get_file_path() { return file_path; }

	stbi_uc* data = nullptr;
	int width = 0;
	int height = 0;
	std::string file_path;
};
