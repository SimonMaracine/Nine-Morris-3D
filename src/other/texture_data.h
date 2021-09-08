#pragma once

#include <string>

#include <stb_image.h>

class TextureData {
public:
	TextureData(const std::string& file_path, bool flip);
	~TextureData();

	stbi_uc* data = nullptr;
	int width;
	int height;
	int channels;
	std::string name;
};
