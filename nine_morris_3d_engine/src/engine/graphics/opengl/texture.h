#pragma once

#include <glad/glad.h>
#include <stb_image.h>

#include "engine/graphics/texture_data.h"
#include "engine/other/encrypt.h"

namespace gl {
    enum class Filter {
        None = 0,
        Linear = GL_LINEAR,
        Nearest = GL_NEAREST
    };

    struct TextureSpecification {
        Filter min_filter = Filter::None;
        Filter mag_filter = Filter::None;

        // Mipmapping is off by default
        bool mipmapping = false;
        float bias = 0.0f;
        int anisotropic_filtering = 0;
    };

    class Texture {
    public:
        Texture(std::string_view file_path, const TextureSpecification& specification);
        Texture(encrypt::EncryptedFile file_path, const TextureSpecification& specification);
        Texture(std::shared_ptr<TextureData> data, const TextureSpecification& specification);
        ~Texture();

        Texture(const Texture&) = delete;
        Texture& operator=(const Texture&) = delete;
        Texture(Texture&&) = delete;
        Texture& operator=(Texture&&) = delete;

        int get_width() { return width; }
        int get_height() { return height; }

        GLuint get_id() { return texture; }

        void bind(GLenum slot);
        static void unbind();
    private:
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

        void bind(GLenum slot);
        static void unbind();
    private:
        GLuint texture = 0;

        std::string name;
    };

    static constexpr float CUBEMAP_VERTICES[] = {
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
