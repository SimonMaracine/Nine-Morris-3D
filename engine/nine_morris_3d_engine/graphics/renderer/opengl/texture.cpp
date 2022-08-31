#include <glad/glad.h>
#include <stb_image.h>
#include <cppblowfish.h>

#include "nine_morris_3d_engine/application/extensions.h"
#include "nine_morris_3d_engine/graphics/debug_opengl.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/texture.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encryption.h"

static std::string get_name(std::string_view file_path) {
    size_t last_slash = file_path.find_last_of("/");
    ASSERT(last_slash != std::string::npos, "Could not find slash");

    return std::string(file_path.substr(last_slash + 1));
}

static std::string get_name_texture3d(const char* file_path) {
    std::vector<std::string> tokens;

    char copy[512];
    strncpy(copy, file_path, 512);

    char* token = strtok(copy, "/");

    while (token != nullptr) {
        tokens.push_back(token);
        token = strtok(nullptr, "/");
    }

    ASSERT(tokens.size() >= 2, "Invalid file path name");

    return tokens[tokens.size() - 2];  // It's ok
}

static void configure_mipmapping(const TextureSpecification& specification) {
    if (specification.mipmapping) {
        const bool anisotropic_filtering_enabled = specification.anisotropic_filtering != 0
                && extensions::extension_supported(extensions::AnisotropicFiltering);

        float bias = specification.bias;

        if (anisotropic_filtering_enabled) {
            bias = 0.0f;
        }

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);

        if (anisotropic_filtering_enabled) {
            float max_amount;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_amount);

            const float amount = std::min(static_cast<float>(specification.anisotropic_filtering), max_amount);

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
        }
    }
}

static void configure_filter_and_wrap(const TextureSpecification& specification) {
    if (specification.mipmapping) {
        ASSERT(specification.min_filter == Filter::None, "Filter must be None");
    } else {
        ASSERT(specification.min_filter != Filter::None, "Filter must not be None");
    }

    const GLint min_filter = specification.mipmapping ? GL_LINEAR_MIPMAP_LINEAR : static_cast<int>(specification.min_filter);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, static_cast<int>(specification.mag_filter));
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
}

Texture::Texture(GLuint texture, int width, int height, std::string_view name)
    : texture(texture), width(width), height(height), name(name) {
    DEB_DEBUG("Created texture {} ({})", texture, name);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);

    DEB_DEBUG("Deleted texture {} ({})", texture, name);
}

std::shared_ptr<Texture> Texture::create(std::string_view file_path, const TextureSpecification& specification) {
    DEB_DEBUG("Loading texture '{}'...", file_path);

    stbi_set_flip_vertically_on_load(1);

    int width, height, channels;
    stbi_uc* data = stbi_load(file_path.data(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture '{}', exiting...", file_path);
        exit(1);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    configure_filter_and_wrap(specification);

    glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    LOG_ALLOCATION(width * height * 4)

    configure_mipmapping(specification);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    std::string name = get_name(file_path);

    return std::make_shared<Texture>(texture, width, height, name);
}

std::shared_ptr<Texture> Texture::create(encryption::EncryptedFile file_path, const TextureSpecification& specification) {
    DEB_DEBUG("Loading texture '{}'...", file_path);

    cppblowfish::Buffer buffer = encryption::load_file(file_path);

    stbi_set_flip_vertically_on_load(1);

    int width, height, channels;
    stbi_uc* data = stbi_load_from_memory(buffer.get(), buffer.size() - buffer.padding(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture '{}', exiting...", file_path);
        exit(1);
    }

    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    configure_filter_and_wrap(specification);

    glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    LOG_ALLOCATION(width * height * 4)

    configure_mipmapping(specification);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    std::string name = get_name(file_path);

    return std::make_shared<Texture>(texture, width, height, name);
}

std::shared_ptr<Texture> Texture::create(std::shared_ptr<TextureData> data, const TextureSpecification& specification) {
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    configure_filter_and_wrap(specification);

    ASSERT(data->data != nullptr, "No data");

    glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, data->width, data->height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, data->width, data->height, GL_RGBA, GL_UNSIGNED_BYTE, data->data);
    LOG_ALLOCATION(data->width * data->height * 4)

    configure_mipmapping(specification);

    glBindTexture(GL_TEXTURE_2D, 0);

    std::string name = get_name(data->file_path);

    return std::make_shared<Texture>(texture, data->width, data->height, name);
}

void Texture::bind(GLenum slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

// --- 3D texture

Texture3D::Texture3D(GLuint texture, std::string_view name)
    : texture(texture), name(name) {
    DEB_DEBUG("Created 3D texture {} ({})", texture, name);
}

Texture3D::~Texture3D() {
    glDeleteTextures(1, &texture);

    DEB_DEBUG("Deleted 3D texture {} ({})", texture, name);
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

    for (size_t i = 0; i < 6; i++) {
        DEB_DEBUG("Loading texture '{}'...", file_paths[i]);

        data = stbi_load(file_paths[i], &width, &height, &channels, 4);

        if (data == nullptr) {
            REL_CRITICAL("Could not load texture '{}', exiting...", file_paths[i]);
            exit(1);
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, width, height,
                0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        LOG_ALLOCATION(width * height * 4)

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

    for (size_t i = 0; i < 6; i++) {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA8, data[i]->width,
                data[i]->height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data[i]->data);
        LOG_ALLOCATION(data[i]->width * data[i]->height * 4)
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    std::string name = get_name_texture3d(data[0]->file_path.c_str());

    return std::make_shared<Texture3D>(texture, name);
}

void Texture3D::bind(GLenum slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}

void Texture3D::unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
