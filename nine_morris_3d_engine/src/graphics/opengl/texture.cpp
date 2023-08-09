#include <memory>
#include <string>
#include <string_view>
#include <array>
#include <optional>
#include <vector>
#include <cstddef>
#include <cstring>

#include <glad/glad.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "engine/application_base/capabilities.hpp"
#include "engine/application_base/panic.hpp"
#include "engine/graphics/opengl/texture.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"
#include "engine/other/encrypt.hpp"

namespace sm {
    static constexpr int CHANNELS = 4;

    static std::string get_name(std::string_view file_path) {
        size_t last_slash = file_path.find_last_of("/");
        SM_ASSERT(last_slash != std::string::npos, "Could not find slash");

        return std::string(file_path.substr(last_slash + 1));
    }

    static std::string get_name_texture3d(const char* file_path) {
        std::vector<std::string> tokens;

        char copy[512];
        std::strncpy(copy, file_path, 512 - 1);

        char* token = std::strtok(copy, "/");

        while (token != nullptr) {
            tokens.push_back(token);
            token = std::strtok(nullptr, "/");
        }

        SM_ASSERT(tokens.size() >= 2, "Invalid file path name");

        return tokens[tokens.size() - 2];  // It's ok
    }

    static bool use_mipmapping(const TextureSpecification& specification) {
        return specification.mipmap_levels > 1;
    }

    static void configure_mipmapping(const TextureSpecification& specification) {
        if (!use_mipmapping(specification)) {
            return;
        }

        const bool anisotropic_filtering_enabled = specification.anisotropic_filtering > 0;

        const float bias = anisotropic_filtering_enabled ? 0.0f : specification.bias;

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);

        if (anisotropic_filtering_enabled) {
            SM_ASSERT(
                specification.anisotropic_filtering <= max_anisotropic_filtering_supported(),
                "Invalid anisotropic filtering value"
            );

            const float amount = static_cast<float>(specification.anisotropic_filtering);

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
        }
    }

    static int filter_to_int(Filter filter) {
        int result = 0;

        switch (filter) {
            case Filter::Linear:
                result = GL_LINEAR;
                break;
            case Filter::Nearest:
                result = GL_NEAREST;
                break;
        }

        return result;
    }

    static int wrap_to_int(Wrap wrap) {
        int result = 0;

        switch (wrap) {
            case Wrap::Repeat:
                result = GL_REPEAT;
                break;
            case Wrap::ClampEdge:
                result = GL_CLAMP_TO_EDGE;
                break;
            case Wrap::ClampBorder:
                result = GL_CLAMP_TO_BORDER;
                break;
        }

        return result;
    }

    static void configure_filter_and_wrap(const TextureSpecification& specification) {
        const int min_filter = (
            use_mipmapping(specification) ? GL_LINEAR_MIPMAP_LINEAR : filter_to_int(specification.min_filter)
        );

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_to_int(specification.mag_filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_to_int(specification.wrap_s));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_to_int(specification.wrap_t));

        if (specification.border_color != std::nullopt) {
            const glm::vec4& color = specification.border_color.value();
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
        }
    }

    static void configure_filter_and_wrap_3d() {
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    }

    GlTexture::GlTexture(std::string_view file_path, const TextureSpecification& specification)
        : specification(specification) {
        LOG_DEBUG("Loading texture `{}`...", file_path);

        stbi_set_flip_vertically_on_load(1);

        int width, height, channels;
        unsigned char* data = stbi_load(file_path.data(), &width, &height, &channels, CHANNELS);

        if (data == nullptr) {
            LOG_DIST_CRITICAL("Could not load texture `{}`", file_path);
            panic();
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        configure_filter_and_wrap(specification);
        allocate_texture(width, height, data);
        configure_mipmapping(specification);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);

        this->width = width;
        this->height = height;
        name = get_name(file_path);

        LOG_DEBUG("Created GL texture {} ({})", texture, name);
    }

    GlTexture::GlTexture(Encrypt::EncryptedFile file_path, const TextureSpecification& specification)
        : specification(specification) {
        LOG_DEBUG("Loading texture `{}`...", file_path);

        const auto [buffer, buffer_size] = Encrypt::load_file(file_path);

        stbi_set_flip_vertically_on_load(1);

        int width, height, channels;
        unsigned char* data = stbi_load_from_memory(
            buffer, buffer_size, &width, &height, &channels, CHANNELS
        );

        if (data == nullptr) {
            LOG_DIST_CRITICAL("Could not load texture `{}`", file_path);
            panic();
        }

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        configure_filter_and_wrap(specification);
        allocate_texture(width, height, data);
        configure_mipmapping(specification);

        glBindTexture(GL_TEXTURE_2D, 0);
        stbi_image_free(data);

        this->width = width;
        this->height = height;
        name = get_name(file_path);

        LOG_DEBUG("Created GL texture {} ({})", texture, name);
    }

    GlTexture::GlTexture(std::shared_ptr<TextureData> data, const TextureSpecification& specification)
        : specification(specification) {
        SM_ASSERT(data->data != nullptr, "No data");

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        configure_filter_and_wrap(specification);
        allocate_texture(data->width, data->height, data->data);
        configure_mipmapping(specification);

        glBindTexture(GL_TEXTURE_2D, 0);

        width = data->width;
        height = data->height;
        name = get_name(data->file_path);

        LOG_DEBUG("Created GL texture {} ({})", texture, name);
    }

    GlTexture::GlTexture(int width, int height, unsigned char* data, const TextureSpecification& specification)
        : specification(specification) {
        SM_ASSERT(data != nullptr, "No data");

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        configure_filter_and_wrap(specification);
        allocate_texture(width, height, data);
        configure_mipmapping(specification);

        glBindTexture(GL_TEXTURE_2D, 0);

        this->width = width;
        this->height = height;
        name = "Unnamed";

        LOG_DEBUG("Created GL texture {} ({})", texture, name);
    }

    GlTexture::~GlTexture() {
        glDeleteTextures(1, &texture);

        LOG_DEBUG("Deleted GL texture {} ({})", texture, name);
    }

    void GlTexture::bind(unsigned int unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void GlTexture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GlTexture::allocate_texture(int width, int height, unsigned char* data) {
        switch (specification.format) {
            case Format::Rgba8:
                glTexStorage2D(GL_TEXTURE_2D, specification.mipmap_levels, GL_RGBA8, width, height);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

                break;
            case Format::R8:
                glTexStorage2D(GL_TEXTURE_2D, specification.mipmap_levels, GL_R8, width, height);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, data);

                break;
        }
    }

    // --- 3D texture

    GlTexture3D::GlTexture3D(const char** file_paths) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        configure_filter_and_wrap_3d();

        stbi_set_flip_vertically_on_load(0);

        int width, height, channels;
        unsigned char* data[6];

        for (size_t i = 0; i < 6; i++) {
            LOG_DEBUG("Loading texture `{}`...", file_paths[i]);

            data[i] = stbi_load(file_paths[i], &width, &height, &channels, CHANNELS);

            if (data[i] == nullptr) {
                LOG_DIST_CRITICAL("Could not load texture `{}`", file_paths[i]);
                panic();
            }
        }

        glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, width, height);

        for (size_t i = 0; i < 6; i++) {
            glTexSubImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, width, height,
                GL_RGBA, GL_UNSIGNED_BYTE, data[i]
            );

            stbi_image_free(data[i]);
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        name = get_name_texture3d(file_paths[0]);

        LOG_DEBUG("Created GL 3D texture {} ({})", texture, name);
    }

    GlTexture3D::GlTexture3D(const std::array<std::shared_ptr<TextureData>, 6>& data) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        configure_filter_and_wrap_3d();

        glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, data[0]->width, data[0]->height);

        for (size_t i = 0; i < 6; i++) {
            glTexSubImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, data[i]->width, data[i]->height,
                GL_RGBA, GL_UNSIGNED_BYTE, data[i]->data
            );
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        name = get_name_texture3d(data[0]->file_path.c_str());

        LOG_DEBUG("Created GL 3D texture {} ({})", texture, name);
    }

    GlTexture3D::~GlTexture3D() {
        glDeleteTextures(1, &texture);

        LOG_DEBUG("Deleted GL 3D texture {} ({})", texture, name);
    }

    void GlTexture3D::bind(unsigned int unit) {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    }

    void GlTexture3D::unbind() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}
