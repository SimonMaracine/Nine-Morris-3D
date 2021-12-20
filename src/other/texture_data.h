#pragma once

#include <string>

#include <stb_image.h>

class TextureData {
public:
	TextureData(const std::string& file_path, bool flip);
	~TextureData();

	stbi_uc* data = nullptr;
	int width = 0;
	int height = 0;
	int channels = 0;
	std::string name;
};
