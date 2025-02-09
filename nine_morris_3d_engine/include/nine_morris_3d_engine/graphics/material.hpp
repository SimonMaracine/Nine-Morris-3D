#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/application/id.hpp"
#include "nine_morris_3d_engine/graphics/opengl/shader.hpp"
#include "nine_morris_3d_engine/graphics/opengl/texture.hpp"

namespace sm {
    class MaterialInstance;

    // Resource representing a material, a shader plus uniforms
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

        explicit Material(std::shared_ptr<GlShader> shader);

        void add_uniform(Uniform type, Id name);
        void add_texture(Id name);
    private:
        std::shared_ptr<GlShader> m_shader;

        std::vector<Id> m_uniforms_mat4;
        std::vector<Id> m_uniforms_int;
        std::vector<Id> m_uniforms_float;
        std::vector<Id> m_uniforms_vec2;
        std::vector<Id> m_uniforms_vec3;
        std::vector<Id> m_uniforms_vec4;
        std::vector<Id> m_textures;

        friend class MaterialInstance;
    };

    // Resource representing an instance of a material, the actual uniform data
    class MaterialInstance {
    public:
        explicit MaterialInstance(std::shared_ptr<Material> material);

        void bind_and_upload() const;

        void set_mat4(Id name, const glm::mat4& matrix);
        void set_int(Id name, int integer);
        void set_float(Id name, float real);
        void set_vec2(Id name, glm::vec2 vector);
        void set_vec3(Id name, glm::vec3 vector);
        void set_vec4(Id name, glm::vec4 vector);
        void set_texture(Id name, std::shared_ptr<GlTexture> texture, int unit);
        void set_texture(Id name, unsigned int texture, int unit);

        const GlShader* get_shader() const;
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

        std::shared_ptr<GlShader> m_shader;

        std::unique_ptr<unsigned char[]> m_data;
        std::size_t m_size {};

        std::unordered_map<Id, Element, Hash> m_offsets;
    };
}
