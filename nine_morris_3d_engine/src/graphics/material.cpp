#include <memory>
#include <unordered_map>
#include <utility>

#include <glm/glm.hpp>
#include <resmanager/resmanager.hpp>

#include "engine/graphics/opengl/shader.hpp"
#include "engine/graphics/material.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    Material::Material(std::shared_ptr<GlShader> shader, int flags)
        : shader(shader), flags(flags) {
        LOG_DEBUG("Created material from shader `{}` with flags `{}`", shader->get_name(), flags);
    }

    Material::~Material() {
        LOG_DEBUG("Deleted material from shader `{}` with flags `{}`", shader->get_name(), flags);
    }

    void Material::add_uniform(Uniform type, Key name) {
        switch (type) {
            case Uniform::Mat4:
                uniforms_mat4[name] = glm::mat4(1.0f);
                break;
            case Uniform::Int:
                uniforms_int[name] = 0;
                break;
            case Uniform::Float:
                uniforms_float[name] = 0.0f;
                break;
            case Uniform::Vec2:
                uniforms_vec2[name] = glm::vec2(0.0f);
                break;
            case Uniform::Vec3:
                uniforms_vec3[name] = glm::vec3(0.0f);
                break;
            case Uniform::Vec4:
                uniforms_vec4[name] = glm::vec4(0.0f);
                break;
            default:
                SM_ASSERT(false, "Unknown uniform type");
        }
    }

    void Material::add_texture(Key name) {
        textures[name] = std::make_pair<int, std::shared_ptr<GlTexture>>(0, {});
    }

    // --- Material instance

    MaterialInstance::MaterialInstance(std::shared_ptr<Material> material) {
        shader = material->shader;
        uniforms_mat4 = material->uniforms_mat4;
        uniforms_int = material->uniforms_int;
        uniforms_float = material->uniforms_float;
        uniforms_vec2 = material->uniforms_vec2;
        uniforms_vec3 = material->uniforms_vec3;
        uniforms_vec4 = material->uniforms_vec4;
        textures = material->textures;
        flags = material->flags;

        LOG_DEBUG("Created material instance");
    }

    MaterialInstance::~MaterialInstance() {
        LOG_DEBUG("Deleted material instance");
    }

    void MaterialInstance::bind() {
        shader->bind();

        for (const auto& [name, matrix] : uniforms_mat4) {
            shader->upload_uniform_mat4(name, matrix);
        }

        for (const auto& [name, value] : uniforms_int) {
            shader->upload_uniform_int(name, value);
        }

        for (const auto& [name, value] : uniforms_float) {
            shader->upload_uniform_float(name, value);
        }

        for (const auto& [name, vector] : uniforms_vec2) {
            shader->upload_uniform_vec2(name, vector);
        }

        for (const auto& [name, vector] : uniforms_vec3) {
            shader->upload_uniform_vec3(name, vector);
        }

        for (const auto& [name, vector] : uniforms_vec4) {
            shader->upload_uniform_vec4(name, vector);
        }

        // Bind any textures
        for (auto& [name, pair] : textures) {
            auto& [unit, texture] = pair;
            shader->upload_uniform_int(name, unit);
            texture->bind(unit);
        }
    }

    void MaterialInstance::set_mat4(Key name, const glm::mat4& matrix) {
        uniforms_mat4.at(name) = matrix;
    }

    void MaterialInstance::set_int(Key name, int value) {
        uniforms_int.at(name) = value;
    }

    void MaterialInstance::set_float(Key name, float value) {
        uniforms_float.at(name) = value;
    }

    void MaterialInstance::set_vec2(Key name, glm::vec2 vector) {
        uniforms_vec2.at(name) = vector;
    }

    void MaterialInstance::set_vec3(Key name, const glm::vec3& vector) {
        uniforms_vec3.at(name) = vector;
    }

    void MaterialInstance::set_vec4(Key name, const glm::vec4& vector) {
        uniforms_vec4.at(name) = vector;
    }

    void MaterialInstance::set_texture(Key name, std::shared_ptr<GlTexture> texture, int unit) {
        textures.at(name) = std::make_pair(unit, texture);
    }
}
