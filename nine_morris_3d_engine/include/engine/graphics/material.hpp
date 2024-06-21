#pragma once

#include <memory>
#include <unordered_map>
#include <cstddef>
#include <vector>

#include <glm/glm.hpp>

#include "engine/application_base/id.hpp"
#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/opengl/texture.hpp"

namespace sm {
    class MaterialInstance;

    class Material {
    public:
        enum class Uniform {
            Mat4,
            Int,
            Float,
            Vec2,
            Vec3,
            Vec4
        };

        enum Flags : unsigned int {
            Outline = 1u << 0,
            DisableBackFaceCulling = 1u << 1,
            CastShadow = 1u << 2
        };

        Material(std::shared_ptr<GlShader> shader, unsigned int flags = 0);

        void add_uniform(Uniform type, Id name);
        void add_texture(Id name);
    private:
        // Materials own shaders
        std::shared_ptr<GlShader> shader;

        std::vector<Id> uniforms_mat4;
        std::vector<Id> uniforms_int;
        std::vector<Id> uniforms_float;
        std::vector<Id> uniforms_vec2;
        std::vector<Id> uniforms_vec3;
        std::vector<Id> uniforms_vec4;
        std::vector<Id> textures;

        unsigned int flags {};

        friend class MaterialInstance;
    };

    class MaterialInstance {
    public:
        explicit MaterialInstance(std::shared_ptr<Material> material);  // TODO create shared_ptr materials?
        ~MaterialInstance();

        MaterialInstance(const MaterialInstance&) = delete;
        MaterialInstance& operator=(const MaterialInstance&) = delete;
        MaterialInstance(MaterialInstance&&) = delete;
        MaterialInstance& operator=(MaterialInstance&&) = delete;

        void bind_and_upload() const;

        void set_mat4(Id name, const glm::mat4& matrix);
        void set_int(Id name, int integer);
        void set_float(Id name, float real);
        void set_vec2(Id name, glm::vec2 vector);
        void set_vec3(Id name, const glm::vec3& vector);
        void set_vec4(Id name, const glm::vec4& vector);
        void set_texture(Id name, std::shared_ptr<GlTexture> texture, int unit);
        void set_texture(Id name, unsigned int texture, int unit);

        const GlShader* get_shader() const { return shader.get(); }

        // It's okay to be public
        unsigned int flags {};
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

            std::size_t offset {};
        };

        struct Texture {
            int unit {};
            unsigned int texture {};
        };

        // Material instances own shaders
        std::shared_ptr<GlShader> shader;

        unsigned char* data {nullptr};
        std::size_t size {};

        std::unordered_map<Id, Element, Hash> offsets;
    };
}
