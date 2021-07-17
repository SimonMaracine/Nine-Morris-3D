#include <memory>
#include <string>

#include <glad/glad.h>
#include <stb_image.h>

#include "opengl/renderer/texture.h"
#include "other/logging.h"

Texture::Texture(GLuint texture, Type type)
        : texture(texture), type(type) {
    SPDLOG_DEBUG("Created texture {}", texture);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);

    SPDLOG_DEBUG("Deleted texture {}", texture);
}

std::shared_ptr<Texture> Texture::create(const std::string& file_path,
                                         Texture::Type type) {
    stbi_set_flip_vertically_on_load(1);

    SPDLOG_DEBUG("Loading texture {}...", file_path.c_str());

    int width, height, channels;
    stbi_uc* data = stbi_load(file_path.c_str(), &width, &height, &channels, 3);  // TODO make this flexible

    if (!data) {
        spdlog::critical("Could not load texture '{}'", file_path.c_str());
        std::exit(1);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB,
                    GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return std::make_shared<Texture>(texture, type);
}

void Texture::bind(GLenum slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

Texture3D::Texture3D(GLuint texture) : texture(texture) {
    SPDLOG_DEBUG("Created 3D texture {}", texture);
}

Texture3D::~Texture3D() {
    glDeleteTextures(1, &texture);

    SPDLOG_DEBUG("Deleted 3D texture {}", texture);    
}

std::shared_ptr<Texture3D> Texture3D::create(const char** file_paths) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(0);

    int width, height, channels;
    stbi_uc* data;

    for (int i = 0; i < 6; i++) {
        SPDLOG_DEBUG("Loading texture {}...", file_paths[i]);

        data = stbi_load(file_paths[i], &width, &height, &channels, 3);  // TODO make this flexible

        if (!data) {
            spdlog::critical("Could not load texture '{}'", file_paths[i]);
            std::exit(1);
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 
                     0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        stbi_image_free(data);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    return std::make_shared<Texture3D>(texture);
}

void Texture3D::bind(GLenum slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}

void Texture3D::unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
