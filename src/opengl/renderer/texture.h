#pragma once

#include <memory>
#include <string>
#include <array>

#include <glad/glad.h>
#include <stb_image.h>

#include "other/texture_data.h"

class Texture {
public:
    Texture(GLuint texture, int width, int height);
    ~Texture();

    static std::shared_ptr<Texture> create(const std::string& file_path, bool mipmapping, float bias = 0.0f);
	static std::shared_ptr<Texture> create(std::shared_ptr<TextureData> data, bool mipmapping, float bias = 0.0f);

	int get_width() const { return width; }
	int get_height() const { return height; }

    void bind(GLenum slot) const;
    static void unbind();

	GLuint get_id() const { return texture; }
private:
    GLuint texture;
	int width, height;
};

class Texture3D {
public:
    Texture3D(GLuint texture);
    ~Texture3D();

    static std::shared_ptr<Texture3D> create(const char** file_paths);
	static std::shared_ptr<Texture3D> create(const std::array<std::shared_ptr<TextureData>, 6>& data);

    void bind(GLenum slot) const;
    static void unbind();
private:
    GLuint texture;
};

template<typename T>
using Rc = std::shared_ptr<T>;

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
