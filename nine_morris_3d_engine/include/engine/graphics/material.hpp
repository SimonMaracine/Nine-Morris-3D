#pragma once

#include <memory>
#include <unordered_map>
#include <utility>

#include <glm/glm.hpp>
#include <resmanager/resmanager.hpp>

#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/opengl/texture.hpp"

namespace sm {
    class Material {
    public:
        using Key = resmanager::HashedStr64;
        using HashFunction = resmanager::Hash<Key>;

        enum class Uniform {
            Mat4,
            Int,
            Float,
            Vec2,
            Vec3,
            Vec4
        };

        enum {
            // FIXME flags here; anything needed?
        };

        Material(std::shared_ptr<GlShader> shader, int flags = 0);
        ~Material();

        Material(const Material&) = delete;
        Material& operator=(const Material&) = delete;
        Material(Material&&) = delete;
        Material& operator=(Material&&) = delete;

        void add_uniform(Uniform type, Key name);
        void add_texture(Key name);
    private:
        std::shared_ptr<GlShader> shader;

        std::unordered_map<Key, glm::mat4, HashFunction> uniforms_mat4;
        std::unordered_map<Key, int, HashFunction> uniforms_int;
        std::unordered_map<Key, float, HashFunction> uniforms_float;
        std::unordered_map<Key, glm::vec2, HashFunction> uniforms_vec2;
        std::unordered_map<Key, glm::vec3, HashFunction> uniforms_vec3;
        std::unordered_map<Key, glm::vec4, HashFunction> uniforms_vec4;

        std::unordered_map<Key, std::pair<int, std::shared_ptr<GlTexture>>, HashFunction> textures;

        int flags = 0;  // FIXME is needed?

        friend class MaterialInstance;
    };

    class MaterialInstance {
    public:
        using Key = Material::Key;
        using HashFunction = Material::HashFunction;

        MaterialInstance(std::shared_ptr<Material> material);
        ~MaterialInstance();

        MaterialInstance(const MaterialInstance&) = delete;
        MaterialInstance& operator=(const MaterialInstance&) = delete;
        MaterialInstance(MaterialInstance&&) = delete;
        MaterialInstance& operator=(MaterialInstance&&) = delete;

        void bind();

        void set_mat4(Key name, const glm::mat4& matrix);
        void set_int(Key name, int value);
        void set_float(Key name, float value);
        void set_vec2(Key name, glm::vec2 vector);
        void set_vec3(Key name, const glm::vec3& vector);
        void set_vec4(Key name, const glm::vec4& vector);
        void set_texture(Key name, std::shared_ptr<GlTexture> texture, int unit);

        std::shared_ptr<GlShader> get_shader() { return shader; }
    private:
        std::shared_ptr<GlShader> shader;

        std::unordered_map<Key, glm::mat4, HashFunction> uniforms_mat4;
        std::unordered_map<Key, int, HashFunction> uniforms_int;
        std::unordered_map<Key, float, HashFunction> uniforms_float;
        std::unordered_map<Key, glm::vec2, HashFunction> uniforms_vec2;
        std::unordered_map<Key, glm::vec3, HashFunction> uniforms_vec3;
        std::unordered_map<Key, glm::vec4, HashFunction> uniforms_vec4;

        std::unordered_map<Key, std::pair<int, std::shared_ptr<GlTexture>>, HashFunction> textures;

        int flags = 0;  // FIXME is needed?
    };
}
