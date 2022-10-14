#include <glad/glad.h>
#include <stb_image.h>
#include <cppblowfish/cppblowfish.h>

#include "nine_morris_3d_engine/application/capabilities.h"
#include "nine_morris_3d_engine/graphics/debug_opengl.h"
#include "nine_morris_3d_engine/graphics/opengl/texture.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"
#include "nine_morris_3d_engine/other/encrypt.h"

static std::string get_name(std::string_view file_path) {
    size_t last_slash = file_path.find_last_of("/");
    ASSERT(last_slash != std::string::npos, "Could not find slash");

    return std::string(file_path.substr(last_slash + 1));
}

static std::string get_name_texture3d(const char* file_path) {
    std::vector<std::string> tokens;

    char copy[512];
    strncpy(copy, file_path, 512 - 1);

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
        const bool anisotropic_filtering_enabled = specification.anisotropic_filtering > 0;

        const float bias = anisotropic_filtering_enabled ? 0.0f : specification.bias;

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);

        if (anisotropic_filtering_enabled) {
            ASSERT(
                specification.anisotropic_filtering <= capabilities::max_anisotropic_filtering_supported(),
                "Invalid anisotropic filtering value"
            );

            const float amount = static_cast<float>(specification.anisotropic_filtering);

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

Texture::Texture(std::string_view file_path, const TextureSpecification& specification) {
    DEB_DEBUG("Loading texture '{}'...", file_path);

    stbi_set_flip_vertically_on_load(1);

    int channels;
    stbi_uc* data = stbi_load(file_path.data(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture '{}', exiting...", file_path);
        exit(1);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    configure_filter_and_wrap(specification);

    glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    LOG_ALLOCATION(width * height * 4)

    configure_mipmapping(specification);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    name = get_name(file_path);

    DEB_DEBUG("Created texture {} ({})", texture, name);
}

Texture::Texture(encrypt::EncryptedFile file_path, const TextureSpecification& specification) {
    DEB_DEBUG("Loading texture '{}'...", file_path);

    cppblowfish::Buffer buffer = encrypt::load_file(file_path);

    stbi_set_flip_vertically_on_load(1);

    int channels;
    stbi_uc* data = stbi_load_from_memory(buffer.get(), buffer.size() - buffer.padding(), &width, &height, &channels, 4);

    if (data == nullptr) {
        REL_CRITICAL("Could not load texture '{}', exiting...", file_path);
        exit(1);
    }

    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    configure_filter_and_wrap(specification);

    glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, width, height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
    LOG_ALLOCATION(width * height * 4)

    configure_mipmapping(specification);

    glBindTexture(GL_TEXTURE_2D, 0);

    stbi_image_free(data);

    name = get_name(file_path);

    DEB_DEBUG("Created texture {} ({})", texture, name);
}

Texture::Texture(std::shared_ptr<TextureData> data, const TextureSpecification& specification) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    configure_filter_and_wrap(specification);

    ASSERT(data->data != nullptr, "No data");

    glTexStorage2D(GL_TEXTURE_2D, 4, GL_RGBA8, data->width, data->height);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, data->width, data->height, GL_RGBA, GL_UNSIGNED_BYTE, data->data);
    LOG_ALLOCATION(data->width * data->height * 4)

    configure_mipmapping(specification);

    glBindTexture(GL_TEXTURE_2D, 0);

    width = data->width;
    height = data->height;
    name = get_name(data->file_path);

    DEB_DEBUG("Created texture {} ({})", texture, name);
}

Texture::~Texture() {
    glDeleteTextures(1, &texture);

    DEB_DEBUG("Deleted texture {} ({})", texture, name);
}

void Texture::bind(GLenum slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, texture);
}

void Texture::unbind() {
    glBindTexture(GL_TEXTURE_2D, 0);
}

// --- 3D texture

Texture3D::Texture3D(const char** file_paths) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    stbi_set_flip_vertically_on_load(0);

    int width, height, channels;
    stbi_uc* data[6];

    for (size_t i = 0; i < 6; i++) {
        DEB_DEBUG("Loading texture '{}'...", file_paths[i]);

        data[i] = stbi_load(file_paths[i], &width, &height, &channels, 4);

        if (data == nullptr) {
            REL_CRITICAL("Could not load texture '{}', exiting...", file_paths[i]);
            exit(1);
        }
    }

    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, width, height);

    for (size_t i = 0; i < 6; i++) {
        glTexSubImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height, GL_RGBA,
            GL_UNSIGNED_BYTE, data[i]
        );
        LOG_ALLOCATION(width * height * 4)

        stbi_image_free(data[i]);
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    name = get_name_texture3d(file_paths[0]);

    DEB_DEBUG("Created 3D texture {} ({})", texture, name);
}

Texture3D::Texture3D(const std::array<std::shared_ptr<TextureData>, 6>& data) {
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, data[0]->width, data[0]->height);

    for (size_t i = 0; i < 6; i++) {
        glTexSubImage2D(
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, data[i]->width, data[i]->height,
            GL_RGBA, GL_UNSIGNED_BYTE, data[i]->data
        );
        LOG_ALLOCATION(data[i]->width * data[i]->height * 4)
    }

    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

    name = get_name_texture3d(data[0]->file_path.c_str());

    DEB_DEBUG("Created 3D texture {} ({})", texture, name);
}

Texture3D::~Texture3D() {
    glDeleteTextures(1, &texture);

    DEB_DEBUG("Deleted 3D texture {} ({})", texture, name);
}

void Texture3D::bind(GLenum slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
}

void Texture3D::unbind() {
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}
