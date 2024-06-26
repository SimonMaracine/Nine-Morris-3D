#pragma once

#include <memory>
#include <optional>
#include <initializer_list>

#include <glm/glm.hpp>

#include "engine/graphics/texture_data.hpp"

namespace sm {
    enum class Filter {
        Linear,
        Nearest
    };

    enum class Wrap {
        Repeat,
        ClampEdge,
        ClampBorder
    };

    enum class Format {
        Rgba8,
        R8
    };

    struct TextureSpecification {
        Format format {Format::Rgba8};

        Filter min_filter {Filter::Linear};
        Filter mag_filter {Filter::Linear};

        Wrap wrap_s {Wrap::ClampBorder};
        Wrap wrap_t {Wrap::ClampBorder};

        std::optional<glm::vec4> border_color;

        // Mipmapping is off by default
        struct {
            int levels {1};
            float bias {};
            int anisotropic_filtering {};
        } mipmapping;
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
        explicit GlTextureCubemap(std::initializer_list<std::shared_ptr<TextureData>> data);
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
