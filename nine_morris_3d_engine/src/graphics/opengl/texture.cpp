#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"

#include <cstddef>
#include <cassert>

#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/graphics/opengl/capabilities.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm {
    static constexpr int CHANNELS {4};

    static bool use_mipmapping(const TextureSpecification& specification) {
        return specification.mipmapping && specification.mipmapping->levels > 1;
    }

    static void configure_mipmapping(const TextureSpecification& specification) {
        if (!use_mipmapping(specification)) {
            return;
        }

        const bool anisotropic_filtering_enabled {specification.mipmapping->anisotropic_filtering > 0};

        const float bias {anisotropic_filtering_enabled ? 0.0f : specification.mipmapping->bias};

        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_LOD_BIAS, bias);

        if (anisotropic_filtering_enabled) {
            const int max_anisotropic_filtering {capabilities::max_anisotropic_filtering_supported()};

            if (specification.mipmapping->anisotropic_filtering > max_anisotropic_filtering) {
                LOG_DIST_WARNING("Invalid anisotropic filtering value: {}", specification.mipmapping->anisotropic_filtering);
            }

            const float amount {
                static_cast<float>(glm::min(specification.mipmapping->anisotropic_filtering, max_anisotropic_filtering))
            };

            glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, amount);
        }
    }

    static int filter_to_int(TextureFilter filter) {
        int result {};

        switch (filter) {
            case TextureFilter::Linear:
                result = GL_LINEAR;
                break;
            case TextureFilter::Nearest:
                result = GL_NEAREST;
                break;
        }

        return result;
    }

    static int wrap_to_int(TextureWrap wrap) {
        int result {};

        switch (wrap) {
            case TextureWrap::Repeat:
                result = GL_REPEAT;
                break;
            case TextureWrap::ClampEdge:
                result = GL_CLAMP_TO_EDGE;
                break;
            case TextureWrap::ClampBorder:
                result = GL_CLAMP_TO_BORDER;
                break;
        }

        return result;
    }

    static void configure_options(const TextureSpecification& specification) {
        const int min_filter {
            use_mipmapping(specification) ? GL_LINEAR_MIPMAP_LINEAR : filter_to_int(specification.min_filter)
        };

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, min_filter);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filter_to_int(specification.mag_filter));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap_to_int(specification.wrap_s));
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap_to_int(specification.wrap_t));

        if (specification.border_color) {
            const glm::vec4 color {*specification.border_color};
            glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, glm::value_ptr(color));
        }
    }

    GlTexture::GlTexture(std::shared_ptr<TextureData> data, const TextureSpecification& specification)
        : specification(specification) {
        assert(data->get_data() != nullptr);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        configure_options(specification);
        allocate_texture(data->get_width(), data->get_height(), data->get_data());
        configure_mipmapping(specification);

        glBindTexture(GL_TEXTURE_2D, 0);

        width = data->get_width();
        height = data->get_height();

        LOG_DEBUG("Created GL texture {}", texture);
    }

    GlTexture::GlTexture(int width, int height, unsigned char* data, const TextureSpecification& specification)
        : specification(specification) {
        assert(data != nullptr);

        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);

        configure_options(specification);
        allocate_texture(width, height, data);
        configure_mipmapping(specification);

        glBindTexture(GL_TEXTURE_2D, 0);

        this->width = width;
        this->height = height;

        LOG_DEBUG("Created GL texture {}", texture);
    }

    GlTexture::~GlTexture() {
        glDeleteTextures(1, &texture);

        LOG_DEBUG("Deleted GL texture {}", texture);
    }

    void GlTexture::bind(unsigned int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_2D, texture);
    }

    void GlTexture::unbind() {
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    void GlTexture::allocate_texture(int width, int height, const unsigned char* data) const {
        const int levels {use_mipmapping(specification) ? specification.mipmapping->levels : 1};

        switch (specification.format) {
            case TextureFormat::Rgba8:
                glTexStorage2D(GL_TEXTURE_2D, levels, GL_RGBA8, width, height);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
                break;
            case TextureFormat::Srgba8:
                glTexStorage2D(GL_TEXTURE_2D, levels, GL_SRGB8, width, height);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
                break;
            case TextureFormat::Srgba8Alpha:
                glTexStorage2D(GL_TEXTURE_2D, levels, GL_SRGB8_ALPHA8, width, height);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);
                break;
            case TextureFormat::R8:
                glTexStorage2D(GL_TEXTURE_2D, levels, GL_R8, width, height);
                glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RED, GL_UNSIGNED_BYTE, data);
                break;
        }
    }

    GlTextureCubemap::GlTextureCubemap(std::initializer_list<std::shared_ptr<TextureData>> data, TextureFormat format) {
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

        const int width {data.begin()[0]->get_width()};
        const int height {data.begin()[0]->get_height()};

        for ([[maybe_unused]] const auto& texture : data) {
            assert(texture->get_width() == width);
            assert(texture->get_height() == height);
        }

        switch (format) {
            case TextureFormat::Rgba8:
                glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, width, height);
                break;
            case TextureFormat::Srgba8:
                glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_SRGB8, width, height);
                break;
            case TextureFormat::Srgba8Alpha:
                glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_SRGB8_ALPHA8, width, height);
                break;
            default:
                assert(false);
                break;
        }

        for (std::size_t i {0}; i < 6; i++) {
            glTexSubImage2D(
                GL_TEXTURE_CUBE_MAP_POSITIVE_X + static_cast<unsigned int>(i),
                0,
                0,
                0,
                data.begin()[i]->get_width(),
                data.begin()[i]->get_height(),
                GL_RGBA,
                GL_UNSIGNED_BYTE,
                data.begin()[i]->get_data()
            );
        }

        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

        LOG_DEBUG("Created GL texture cubemap {}", texture);
    }

    GlTextureCubemap::~GlTextureCubemap() {
        glDeleteTextures(1, &texture);

        LOG_DEBUG("Deleted GL texture cubemap {}", texture);
    }

    void GlTextureCubemap::bind(unsigned int unit) const {
        glActiveTexture(GL_TEXTURE0 + unit);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    }

    void GlTextureCubemap::unbind() {
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    }
}
