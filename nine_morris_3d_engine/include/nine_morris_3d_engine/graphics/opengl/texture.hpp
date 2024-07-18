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
        Rgba8,
        Srgba8,
        Srgba8Alpha,
        R8
    };

    struct TextureMipmapping {
        int levels {1};
        float bias {};
        int anisotropic_filtering {};
    };

    struct TextureSpecification {
        TextureFormat format {TextureFormat::Rgba8};

        TextureFilter min_filter {TextureFilter::Linear};
        TextureFilter mag_filter {TextureFilter::Linear};

        TextureWrap wrap_s {TextureWrap::ClampBorder};
        TextureWrap wrap_t {TextureWrap::ClampBorder};

        std::optional<glm::vec4> border_color;
        std::optional<TextureMipmapping> mipmapping;
    };

    class GlTexture {
    public:
        GlTexture(std::shared_ptr<TextureData> data, const TextureSpecification& specification = {});
        GlTexture(int width, int height, unsigned char* data, const TextureSpecification& specification = {});
        ~GlTexture();

        GlTexture(const GlTexture&) = delete;
        GlTexture& operator=(const GlTexture&) = delete;
        GlTexture(GlTexture&&) = delete;
        GlTexture& operator=(GlTexture&&) = delete;

        int get_width() const { return width; }
        int get_height() const { return height; }
        unsigned int get_id() const { return texture; }

        void bind(unsigned int unit) const;
        static void unbind();
    private:
        void allocate_texture(int width, int height, const unsigned char* data) const;

        TextureSpecification specification;

        unsigned int texture {};
        int width {};
        int height {};
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
        unsigned int texture {};
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
