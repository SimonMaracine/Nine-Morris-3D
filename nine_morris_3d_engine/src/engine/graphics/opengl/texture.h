#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "engine/graphics/texture_data.h"
#include "engine/other/encrypt.h"

namespace gl {
    enum class Filter {
        Linear = GL_LINEAR,
        Nearest = GL_NEAREST
    };

    enum class Wrap {
        Repeat = GL_REPEAT,
        ClampEdge = GL_CLAMP_TO_EDGE,
        ClampBorder = GL_CLAMP_TO_BORDER
    };

    enum class Format {
        Rgba8,
        R8
    };

    struct TextureSpecification {
        Format format = Format::Rgba8;

        Filter min_filter = Filter::Linear;
        Filter mag_filter = Filter::Linear;

        Wrap wrap_s = Wrap::ClampBorder;
        Wrap wrap_t = Wrap::ClampBorder;

        std::optional<glm::vec4> border_color = std::nullopt;

        // Mipmapping is off by default
        int mipmap_levels = 1;
        float bias = 0.0f;
        int anisotropic_filtering = 0;
    };

    class Texture {
    public:
        Texture(std::string_view file_path, const TextureSpecification& specification);
        Texture(Encrypt::EncryptedFile file_path, const TextureSpecification& specification);
        Texture(std::shared_ptr<TextureData> data, const TextureSpecification& specification);
        Texture(int width, int height, unsigned char* data, const TextureSpecification& specification);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&) = delete;
        Texture& operator=(Texture&&) = delete;

        int get_width() { return width; }
        int get_height() { return height; }
        GLuint get_id() { return texture; }

        void bind(GLenum unit);
        static void unbind();
    private:
        void allocate_texture(int width, int height, unsigned char* data);

        TextureSpecification specification;

        GLuint texture = 0;
        int width = 0;
        int height = 0;

        std::string name;
    };

    class Texture3D {
    public:
        Texture3D(const char** file_paths);  // Don't need encrypted version
        Texture3D(const std::array<std::shared_ptr<TextureData>, 6>& data);
        ~Texture3D();

        Texture3D(const Texture3D&) = delete;
        Texture3D& operator=(const Texture3D&) = delete;
        Texture3D(Texture3D&&) = delete;
        Texture3D& operator=(Texture3D&&) = delete;

        void bind(GLenum unit);
        static void unbind();
    private:
        GLuint texture = 0;

        std::string name;
    };

    inline constexpr float CUBEMAP_VERTICES[] = {
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
