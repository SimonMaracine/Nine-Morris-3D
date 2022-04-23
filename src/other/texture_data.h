#pragma once

#include <string>
#include <string_view>

#include <stb_image.h>

/**
 * Object representing raw texture data.
 * Pass this to OpenGL texture objects.
 */
class TextureData {
public:
	TextureData(std::string_view file_path, bool flip);
	~TextureData();

	stbi_uc* data = nullptr;
	int width = 0;
	int height = 0;
	int channels = 0;
	std::string file_path;
};
