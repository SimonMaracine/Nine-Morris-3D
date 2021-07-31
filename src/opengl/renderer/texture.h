#pragma once

#include <memory>
#include <string>
#include <array>

#include <glad/glad.h>
#include <stb_image.h>

class TextureData {
public:
	TextureData(const std::string& file_path, bool flip);
	~TextureData();

	stbi_uc* data = nullptr;
	int width;
	int height;
	int channels;
};

class Texture {
public:
    Texture(GLuint texture);
    ~Texture();

    static std::shared_ptr<Texture> create(const std::string& file_path);
	static std::shared_ptr<Texture> create(std::shared_ptr<TextureData> data);

    void bind(GLenum slot) const;
    static void unbind();
private:
    GLuint texture;
};

class Texture3D {
public:
    Texture3D(GLuint texture);
    ~Texture3D();

    static std::shared_ptr<Texture3D> create(const char** file_paths);
	static std::shared_ptr<Texture3D> create(std::array<std::shared_ptr<TextureData>, 6> data);

    void bind(GLenum slot) const;
    static void unbind();
private:
    GLuint texture;
};

constexpr float cube_map_points[] = {
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
