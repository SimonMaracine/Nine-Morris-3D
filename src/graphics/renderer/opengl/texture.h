#pragma once

#include <glad/glad.h>
#include <stb_image.h>

#include "other/texture_data.h"
#include "other/encryption.h"

class Texture {
public:
    Texture(GLuint texture, int width, int height, std::string_view name);
    ~Texture();

    static std::shared_ptr<Texture> create(std::string_view file_path, bool mipmapping,
			float bias = 0.0f, int anisotropic_filtering = 0);
	static std::shared_ptr<Texture> create(const encryption::EncryptedFile& file_path, bool mipmapping,
			float bias = 0.0f, int anisotropic_filtering = 0);
	static std::shared_ptr<Texture> create(std::shared_ptr<TextureData> data, bool mipmapping,
			float bias = 0.0f, int anisotropic_filtering = 0);

	int get_width() { return width; }
	int get_height() { return height; }

    void bind(GLenum slot);
    static void unbind();

	GLuint get_id() { return texture; }
private:
    GLuint texture = 0;
	int width = 0, height = 0;

	std::string name;
};

class Texture3D {
public:
    Texture3D(GLuint texture, std::string_view name);
    ~Texture3D();

    static std::shared_ptr<Texture3D> create(const char** file_paths);  // Don't need encrypted version
	static std::shared_ptr<Texture3D> create(const std::array<std::shared_ptr<TextureData>, 6>& data);

    void bind(GLenum slot);
    static void unbind();
private:
    GLuint texture = 0;

	std::string name;
};

constexpr float SKYBOX_VERTICES[] = {
    -5.0f,  5.0f, -5.0f,
	-5.0f, -5.0f, -5.0f,
	 5.0f, -5.0f, -5.0f,
	 5.0f, -5.0f, -5.0f,
	 5.0f,  5.0f, -5.0f,
	-5.0f,  5.0f, -5.0f,

	-5.0f, -5.0f,  5.0f,
	-5.0f, -5.0f, -5.0f,
	-5.0f,  5.0f, -5.0f,
	-5.0f,  5.0f, -5.0f,
	-5.0f,  5.0f,  5.0f,
	-5.0f, -5.0f,  5.0f,

	 5.0f, -5.0f, -5.0f,
	 5.0f, -5.0f,  5.0f,
	 5.0f,  5.0f,  5.0f,
	 5.0f,  5.0f,  5.0f,
	 5.0f,  5.0f, -5.0f,
	 5.0f, -5.0f, -5.0f,

	-5.0f, -5.0f,  5.0f,
	-5.0f,  5.0f,  5.0f,
	 5.0f,  5.0f,  5.0f,
	 5.0f,  5.0f,  5.0f,
	 5.0f, -5.0f,  5.0f,
	-5.0f, -5.0f,  5.0f,

	-5.0f,  5.0f, -5.0f,
	 5.0f,  5.0f, -5.0f,
	 5.0f,  5.0f,  5.0f,
	 5.0f,  5.0f,  5.0f,
	-5.0f,  5.0f,  5.0f,
	-5.0f,  5.0f, -5.0f,

	-5.0f, -5.0f, -5.0f,
	-5.0f, -5.0f,  5.0f,
	 5.0f, -5.0f, -5.0f,
	 5.0f, -5.0f, -5.0f,
	-5.0f, -5.0f,  5.0f,
	 5.0f, -5.0f,  5.0f
};
