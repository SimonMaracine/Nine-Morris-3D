#include <memory>
#include <string>
#include <cassert>
#include <vector>
#include <string.h>

#include <glad/glad.h>
#include <stb_image.h>

#include "opengl/renderer/texture.h"
#include "opengl/debug_opengl.h"
#include "other/logging.h"

static std::string get_name(const std::string& file_path) {
    size_t last_slash = file_path.find_last_of("/");
    assert(last_slash != std::string::npos);

    return file_path.substr(last_slash + 1);
}

static std::string get_name_texture3d(const char* file_path) {
    std::vector<std::string> tokens;

    char copy[256];
    strcpy(copy, file_path);

    char* token = strtok(copy, "/");

    while (token != nullptr) {
        tokens.push_back(token);
        token = strtok(nullptr, "/");
    }

    assert(tokens.size() >= 2);

    return tokens[tokens.size() - 2];  // It's ok
}

Texture::Texture(GLuint texture, int width, int height, const std::string& name)
    : texture(texture), width(width), height(height), name(name) {
    SPDLOG_DEBUG("Created texture {} ({})", texture, name.c_str());
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);

    SPDLOG_DEBUG("Deleted texture {} ({})", texture, name.c_str());
}

std::shared_ptr<Texture> Texture::create(const std::string& file_path, bool mipmapping, float bias) {
    SPDLOG_DEBUG("Loading texture '{}'...", file_path.c_str());

    stbi_set_flip_vertically_on_load(1);

    int width, height, channels;
    stbi_uc* data = stbi_load(file_path.c_str(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture '{}'", file_path.c_str());
        std::exit(1);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (mipmapping)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    LOG_ALLOCATION(width * height * 4);

    if (mipmapping) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    std::string name = get_name(file_path);

    return std::make_shared<Texture>(texture, width, height, name);
}

std::shared_ptr<Texture> Texture::create(std::shared_ptr<TextureData> data, bool mipmapping, float bias) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    if (mipmapping)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    else
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    assert(data->data != nullptr);

    glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, data->width, data->height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, data->width, data->height, GL_RGBA,
            GL_UNSIGNED_BYTE, data->data);
    LOG_ALLOCATION(data->width * data->height * 4);

    if (mipmapping) {
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);
    }

    glBindTexture(GL_TEXTURE_2D, 0);

    std::string name = get_name(data->file_path);

    return std::make_shared<Texture>(texture, data->width, data->height, name);
}

void Texture::bind(GLenum slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

// --- 3D texture

Texture3D::Texture3D(GLuint texture, const std::string& name)
    : texture(texture), name(name) {
    SPDLOG_DEBUG("Created 3D texture {} ({})", texture, name.c_str());
}

Texture3D::~Texture3D() {
    glDeleteTextures(1, &texture);

    SPDLOG_DEBUG("Deleted 3D texture {} ({})", texture, name.c_str());
}

std::shared_ptr<Texture3D> Texture3D::create(const char** file_paths) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(0);

    int width, height, channels;
    stbi_uc* data;

    for (unsigned int i = 0; i < 6; i++) {
        SPDLOG_DEBUG("Loading texture '{}'...", file_paths[i]);

        data = stbi_load(file_paths[i], &width, &height, &channels, 4);

        if (data == nullptr) {
            REL_CRITICAL("Could not load texture '{}'", file_paths[i]);
            std::exit(1);
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        LOG_ALLOCATION(width * height * 4);

        stbi_image_free(data);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    std::string name = get_name_texture3d(file_paths[0]);

    return std::make_shared<Texture3D>(texture, name);
}

std::shared_ptr<Texture3D> Texture3D::create(const std::array<std::shared_ptr<TextureData>, 6>& data) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    for (unsigned int i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, data[i]->width,
                data[i]->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[i]->data);
        LOG_ALLOCATION(data[i]->width * data[i]->height * 4);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    std::string name = get_name_texture3d(data[0]->file_path.c_str());

    return std::make_shared<Texture3D>(texture, name);
}

void Texture3D::bind(GLenum slot) const {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}

void Texture3D::unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
