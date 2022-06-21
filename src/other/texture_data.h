#pragma once

#include <stb_image.h>

#include "other/encryption.h"

struct GLFWimage;

/**
 * Object representing raw texture data.
 * Pass this to OpenGL texture objects.
 */
class TextureData {
public:
	TextureData(std::string_view file_path, bool flip);
	TextureData(const encryption::EncryptedFile& file_path, bool flip);
	~TextureData();

	GLFWimage get_data_glfw() const;
	std::string_view get_file_path() const { return file_path; }

	stbi_uc* data = nullptr;
	int width = 0;
	int height = 0;
	int channels = 0;
	std::string file_path;
};
