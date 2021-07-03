#include <memory>
#include <string>

#include <glad/glad.h>
#include <stb_image.h>

#include "opengl/renderer/texture.h"
#include "logging.h"

Texture::Texture(GLuint texture) : texture(texture) {
    SPDLOG_DEBUG("Created texture {}", texture);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);

    SPDLOG_DEBUG("Deleted texture {}", texture);
}

std::shared_ptr<Texture> Texture::create(const std::string& file_path) {
    stbi_set_flip_vertically_on_load(1);

    int width, height, channels;
    stbi_uc* data = stbi_load(file_path.c_str(), &width, &height, &channels, 3);  // TODO make this flexible

    if (!data) {
        spdlog::critical("Could not load texture '{}'", file_path.c_str());
        std::exit(1);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGB,
                    GL_UNSIGNED_BYTE, data);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    return std::make_shared<Texture>(texture);
}

void Texture::bind(GLenum slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}
