#pragma once

#include <memory>
#include <string>
#include <string_view>
#include <array>
#include <optional>

#include <glm/glm.hpp>

#include "engine/graphics/texture_data.hpp"
#include "engine/other/encrypt.hpp"

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
        unsigned int get_id() { return texture; }

        void bind(unsigned int unit);
        static void unbind();
    private:
        void allocate_texture(int width, int height, unsigned char* data);

        TextureSpecification specification;

        unsigned int texture = 0;
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

        void bind(unsigned int unit);
        static void unbind();
    private:
        unsigned int texture = 0;

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
