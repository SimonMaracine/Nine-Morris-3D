#pragma once

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

	TextureData(const TextureData&) = delete;
    TextureData& operator=(const TextureData&) = delete;
    TextureData(TextureData&&) = delete;
    TextureData& operator=(TextureData&&) = delete;

	Image get_data();
	std::string_view get_file_path() { return file_path; }
private:
	unsigned char* data = nullptr;
	int width = 0;
	int height = 0;
	std::string file_path;

	friend class Texture;
	friend class Texture3D;
};
