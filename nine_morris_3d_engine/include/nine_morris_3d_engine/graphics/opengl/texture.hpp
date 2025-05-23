#pragma once

#include <memory>
#include <optional>
#include <initializer_list>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/graphics/texture_data.hpp"

namespace sm {
    enum class TextureFilter {
        Linear,
        Nearest
    };

    enum class TextureWrap {
        Repeat,
        ClampEdge,
        ClampBorder
    };

    enum class TextureFormat {
        Rgb8,
        Rgba8,
        Srgb8,
        Srgb8Alpha8,
        R8
    };

    // Mipmapping parameters
    struct TextureMipmapping {
        int levels {4};
        float bias {0.0f};
        int anisotropic_filtering {0};
    };

    // Texture parameters
    struct TextureSpecification {
        TextureFormat format {TextureFormat::Srgb8Alpha8};

        TextureFilter min_filter {TextureFilter::Linear};
        TextureFilter mag_filter {TextureFilter::Linear};

        TextureWrap wrap_s {TextureWrap::ClampBorder};
        TextureWrap wrap_t {TextureWrap::ClampBorder};

        std::optional<glm::vec4> border_color;
        std::optional<TextureMipmapping> mipmapping;
    };

    // OpenGL resource representing a texture, an image plus metadata
    class GlTexture {
    public:
        GlTexture(std::shared_ptr<TextureData> data, const TextureSpecification& specification = {});
        GlTexture(int width, int height, unsigned char* data, const TextureSpecification& specification = {});
        ~GlTexture();

        GlTexture(const GlTexture&) = delete;
        GlTexture& operator=(const GlTexture&) = delete;
        GlTexture(GlTexture&&) = delete;
        GlTexture& operator=(GlTexture&&) = delete;

        int get_width() const;
        int get_height() const;
        unsigned int get_id() const;

        void bind(unsigned int unit) const;
        static void unbind();
    private:
        void allocate_texture(int width, int height, const unsigned char* data) const;

        TextureSpecification m_specification;

        unsigned int m_texture {};
        int m_width {};
        int m_height {};
    };

    class GlTextureCubemap {
    public:
        GlTextureCubemap(std::initializer_list<std::shared_ptr<TextureData>> data, TextureFormat format = TextureFormat::Rgba8);
        ~GlTextureCubemap();

        GlTextureCubemap(const GlTextureCubemap&) = delete;
        GlTextureCubemap& operator=(const GlTextureCubemap&) = delete;
        GlTextureCubemap(GlTextureCubemap&&) = delete;
        GlTextureCubemap& operator=(GlTextureCubemap&&) = delete;

        void bind(unsigned int unit) const;
        static void unbind();
    private:
        unsigned int m_texture {};
    };

    inline constexpr float CUBEMAP_VERTICES[] {
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
}
