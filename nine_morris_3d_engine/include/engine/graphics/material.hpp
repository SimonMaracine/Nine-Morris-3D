#pragma once

#include <memory>
#include <unordered_map>
#include <cstddef>
#include <vector>

#include <glm/glm.hpp>
#include <resmanager/resmanager.hpp>

#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/opengl/texture.hpp"

namespace sm {
    class Material {
    public:
        using Key = resmanager::HashedStr64;

        enum class Uniform {
            Mat4,
            Int,
            Float,
            Vec2,
            Vec3,
            Vec4
        };

        enum Flags : unsigned int {
            Outline = 1 << 0
        };

        Material(std::shared_ptr<GlShader> shader, unsigned int flags = 0);
        ~Material();

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;
        Material(Material&&) = delete;
        Material& operator=(Material&&) = delete;

        void add_uniform(Uniform type, Key name);
        void add_texture(Key name);
    private:
        std::shared_ptr<GlShader> shader;

        std::vector<Key> uniforms_mat4;
        std::vector<Key> uniforms_int;
        std::vector<Key> uniforms_float;
        std::vector<Key> uniforms_vec2;
        std::vector<Key> uniforms_vec3;
        std::vector<Key> uniforms_vec4;
        std::vector<Key> textures;

        unsigned int flags = 0;

        friend class MaterialInstance;
    };

    class MaterialInstance {
    public:
        using Key = Material::Key;
        using KeyHash = resmanager::Hash<Key>;

        MaterialInstance(std::shared_ptr<Material> material);
        ~MaterialInstance();

        MaterialInstance(const MaterialInstance&) = delete;
        MaterialInstance& operator=(const MaterialInstance&) = delete;
        MaterialInstance(MaterialInstance&&) = delete;
        MaterialInstance& operator=(MaterialInstance&&) = delete;

        void bind_and_upload();

        void set_mat4(Key name, const glm::mat4& matrix);
        void set_int(Key name, int integer);
        void set_float(Key name, float real);
        void set_vec2(Key name, glm::vec2 vector);
        void set_vec3(Key name, const glm::vec3& vector);
        void set_vec4(Key name, const glm::vec4& vector);
        void set_texture(Key name, std::shared_ptr<GlTexture> texture, int unit);
        void set_texture(Key name, unsigned int texture, int unit);

        std::shared_ptr<GlShader> get_shader() { return shader; }  // TODO maybe const pointer

        // It's left public
        unsigned int flags = 0;
    private:
        struct Element {
            enum class Type {
                Mat4,
                Int,
                Float,
                Vec2,
                Vec3,
                Vec4,
                Texture
            } type {};

            std::size_t offset = 0;
        };

        struct Texture {
            int unit = 0;
            unsigned int texture = 0;
        };

        std::shared_ptr<GlShader> shader;

        unsigned char* data = nullptr;
        std::size_t size = 0;

        std::unordered_map<Key, Element, KeyHash> offsets;
    };
}
