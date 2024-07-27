#include "nine_morris_3d_engine/graphics/material.hpp"

#include <cstring>

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/graphics/internal/opengl.hpp"

namespace sm {
    Material::Material(std::shared_ptr<GlShader> shader, unsigned int flags) noexcept
        : shader(shader), flags(flags) {
        LOG_DEBUG("Created material from shader {} with flags {}", shader->get_id(), flags);
    }

    void Material::add_uniform(Uniform type, Id name) {
        switch (type) {
            case Uniform::Mat4:
                uniforms_mat4.push_back(name);
                break;
            case Uniform::Int:
                uniforms_int.push_back(name);
                break;
            case Uniform::Float:
                uniforms_float.push_back(name);
                break;
            case Uniform::Vec2:
                uniforms_vec2.push_back(name);
                break;
            case Uniform::Vec3:
                uniforms_vec3.push_back(name);
                break;
            case Uniform::Vec4:
                uniforms_vec4.push_back(name);
                break;
        }
    }

    void Material::add_texture(Id name) {
        textures.push_back(name);
    }

    MaterialInstance::MaterialInstance(std::shared_ptr<Material> material) {
        shader = material->shader;
        flags = material->flags;

        std::size_t offset {};

        for (const auto& name : material->uniforms_mat4) {
            Element element;
            element.type = Element::Type::Mat4;
            element.offset = offset;

            offsets[name] = element;
            offset += sizeof(glm::mat4);
        }

        for (const auto& name : material->uniforms_int) {
            Element element;
            element.type = Element::Type::Int;
            element.offset = offset;

            offsets[name] = element;
            offset += sizeof(int);
        }

        for (const auto& name : material->uniforms_float) {
            Element element;
            element.type = Element::Type::Float;
            element.offset = offset;

            offsets[name] = element;
            offset += sizeof(float);
        }

        for (const auto& name : material->uniforms_vec2) {
            Element element;
            element.type = Element::Type::Vec2;
            element.offset = offset;

            offsets[name] = element;
            offset += sizeof(glm::vec2);
        }

        for (const auto& name : material->uniforms_vec3) {
            Element element;
            element.type = Element::Type::Vec3;
            element.offset = offset;

            offsets[name] = element;
            offset += sizeof(glm::vec3);
        }

        for (const auto& name : material->uniforms_vec4) {
            Element element;
            element.type = Element::Type::Vec4;
            element.offset = offset;

            offsets[name] = element;
            offset += sizeof(glm::vec4);
        }

        for (const auto& name : material->textures) {
            Element element;
            element.type = Element::Type::Texture;
            element.offset = offset;

            offsets[name] = element;
            offset += sizeof(Texture);
        }

        size = offset;
        data = std::make_unique<unsigned char[]>(size);

        LOG_DEBUG("Created material instance from shader {}", shader->get_id());
    }

    void MaterialInstance::bind_and_upload() const {
        shader->bind();

        for (const auto& [name, element] : offsets) {
            switch (element.type) {
                case Element::Type::Mat4: {
                    glm::mat4 matrix {};
                    std::memcpy(&matrix, data.get() + element.offset, sizeof(matrix));

                    shader->upload_uniform_mat4(name, matrix);

                    break;
                }
                case Element::Type::Int: {
                    int integer {};
                    std::memcpy(&integer, data.get() + element.offset, sizeof(integer));

                    shader->upload_uniform_int(name, integer);

                    break;
                }
                case Element::Type::Float: {
                    float real {};
                    std::memcpy(&real, data.get() + element.offset, sizeof(real));

                    shader->upload_uniform_float(name, real);

                    break;
                }
                case Element::Type::Vec2: {
                    glm::vec2 vector {};
                    std::memcpy(&vector, data.get() + element.offset, sizeof(vector));

                    shader->upload_uniform_vec2(name, vector);

                    break;
                }
                case Element::Type::Vec3: {
                    glm::vec3 vector {};
                    std::memcpy(&vector, data.get() + element.offset, sizeof(vector));

                    shader->upload_uniform_vec3(name, vector);

                    break;
                }
                case Element::Type::Vec4: {
                    glm::vec4 vector {};
                    std::memcpy(&vector, data.get() + element.offset, sizeof(vector));

                    shader->upload_uniform_vec4(name, vector);

                    break;
                }
                case Element::Type::Texture: {
                    Texture texture {};
                    std::memcpy(&texture, data.get() + element.offset, sizeof(texture));

                    shader->upload_uniform_int(name, texture.unit);
                    internal::opengl::bind_texture_2d(texture.texture, texture.unit);

                    break;
                }
            }
        }
    }

    void MaterialInstance::set_mat4(Id name, const glm::mat4& matrix) {
        const Element& element {offsets.at(name)};
        std::memcpy(data.get() + element.offset, &matrix, sizeof(matrix));
    }

    void MaterialInstance::set_int(Id name, int integer) {
        const Element& element {offsets.at(name)};
        std::memcpy(data.get() + element.offset, &integer, sizeof(integer));
    }

    void MaterialInstance::set_float(Id name, float real) {
        const Element& element {offsets.at(name)};
        std::memcpy(data.get() + element.offset, &real, sizeof(real));
    }

    void MaterialInstance::set_vec2(Id name, glm::vec2 vector) {
        const Element& element {offsets.at(name)};
        std::memcpy(data.get() + element.offset, &vector, sizeof(vector));
    }

    void MaterialInstance::set_vec3(Id name, glm::vec3 vector) {
        const Element& element {offsets.at(name)};
        std::memcpy(data.get() + element.offset, &vector, sizeof(vector));
    }

    void MaterialInstance::set_vec4(Id name, glm::vec4 vector) {
        const Element& element {offsets.at(name)};
        std::memcpy(data.get() + element.offset, &vector, sizeof(vector));
    }

    void MaterialInstance::set_texture(Id name, std::shared_ptr<GlTexture> texture, int unit) {
        set_texture(name, texture->get_id(), unit);
    }

    void MaterialInstance::set_texture(Id name, unsigned int texture, int unit) {
        Texture result_texure {};
        result_texure.unit = unit;
        result_texure.texture = texture;

        const Element& element {offsets.at(name)};
        std::memcpy(data.get() + element.offset, &result_texure, sizeof(result_texure));
    }

    const GlShader* MaterialInstance::get_shader() const noexcept {
        return shader.get();
    }
}
