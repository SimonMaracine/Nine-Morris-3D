#pragma once

#include <stb_image.h>

#include "nine_morris_3d_engine/other/encrypt.h"

struct GLFWimage;

class TextureData {
public:
	TextureData(std::string_view file_path, bool flip);
	TextureData(encrypt::EncryptedFile file_path, bool flip);
	~TextureData();

	GLFWimage get_data_glfw() const;
	std::string_view get_file_path() const { return file_path; }

	stbi_uc* data = nullptr;
	int width = 0;
	int height = 0;
	int channels = 0;
	std::string file_path;
};
