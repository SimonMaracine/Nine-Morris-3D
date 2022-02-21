#pragma once

#include <memory>
#include <string>
#include <array>

#include <glad/glad.h>
#include <stb_image.h>

#include "other/texture_data.h"

/**
 * These represent OpenGL textures in video memory.
 */
class Texture {
public:
    Texture(GLuint texture, int width, int height, const std::string& name);
    ~Texture();

    static std::shared_ptr<Texture> create(const std::string& file_path, bool mipmapping, float bias = 0.0f);
	static std::shared_ptr<Texture> create(std::shared_ptr<TextureData> data, bool mipmapping, float bias = 0.0f);

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
    Texture3D(GLuint texture, const std::string& name);
    ~Texture3D();

    static std::shared_ptr<Texture3D> create(const char** file_paths);
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
