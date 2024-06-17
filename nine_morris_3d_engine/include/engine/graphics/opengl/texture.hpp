#pragma once

#include <memory>
#include <string>
#include <array>
#include <optional>

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
            float bias {0.0f};
            int anisotropic_filtering {0};
        } mipmapping;
    };

    class GlTexture {
    public:
        GlTexture(const std::string& file_path, const TextureSpecification& specification);
        GlTexture(std::shared_ptr<TextureData> data, const TextureSpecification& specification);
        GlTexture(int width, int height, unsigned char* data, const TextureSpecification& specification);
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

        unsigned int texture {0};
        int width {0};
        int height {0};

        std::string name;
    };

    class GlTextureCubemap {
    public:
        GlTextureCubemap(const char** file_paths);  // Don't need encrypted version
        GlTextureCubemap(const std::array<std::shared_ptr<TextureData>, 6>& data);
        ~GlTextureCubemap();

        GlTextureCubemap(const GlTextureCubemap&) = delete;
        GlTextureCubemap& operator=(const GlTextureCubemap&) = delete;
        GlTextureCubemap(GlTextureCubemap&&) = delete;
        GlTextureCubemap& operator=(GlTextureCubemap&&) = delete;

        void bind(unsigned int unit) const;
        static void unbind();
    private:
        unsigned int texture {0};

        std::string name;
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
