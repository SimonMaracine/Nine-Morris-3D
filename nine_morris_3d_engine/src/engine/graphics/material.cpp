#include <glm/glm.hpp>

#include "engine/graphics/material.h"
#include "engine/graphics/opengl/shader.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"

Material::Material(std::shared_ptr<gl::Shader> shader, int flags)
    : shader(shader), flags(flags) {
    DEB_DEBUG("Created material from shader `{}` with flags `{}`", shader->get_name(), flags);
}

Material::~Material() {
    DEB_DEBUG("Deleted material from shader `{}` with flags `{}`", shader->get_name(), flags);
}

void Material::add_uniform(Uniform type, std::string_view name) {
    switch (type) {
        case Uniform::Mat4:
            uniforms_mat4[std::string(name)] = glm::mat4(1.0f);
            break;
        case Uniform::Int:
            uniforms_int[std::string(name)] = 0;
            break;
        case Uniform::Float:
            uniforms_float[std::string(name)] = 0.0f;
            break;
        case Uniform::Vec2:
            uniforms_vec2[std::string(name)] = glm::vec2(0.0f);
            break;
        case Uniform::Vec3:
            uniforms_vec3[std::string(name)] = glm::vec3(0.0f);
            break;
        case Uniform::Vec4:
            uniforms_vec4[std::string(name)] = glm::vec4(0.0f);
            break;
        default:
            ASSERT(false, "Unknown uniform type");
    }
}

void Material::add_texture(std::string_view name) {
    textures[std::string(name)] = std::make_pair<int, std::shared_ptr<gl::Texture>>(0, {});
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

    DEB_DEBUG("Created material instance");
}

MaterialInstance::~MaterialInstance() {
    DEB_DEBUG("Deleted material instance");
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

void MaterialInstance::set_mat4(std::string_view name, const glm::mat4& matrix) {
    uniforms_mat4[std::string(name)] = matrix;
}

void MaterialInstance::set_int(std::string_view name, int value) {
    uniforms_int[std::string(name)] = value;
}

void MaterialInstance::set_float(std::string_view name, float value) {
    uniforms_float[std::string(name)] = value;
}

void MaterialInstance::set_vec2(std::string_view name, glm::vec2 vector) {
    uniforms_vec2[std::string(name)] = vector;
}

void MaterialInstance::set_vec3(std::string_view name, const glm::vec3& vector) {
    uniforms_vec3[std::string(name)] = vector;
}

void MaterialInstance::set_vec4(std::string_view name, const glm::vec4& vector) {
    uniforms_vec4[std::string(name)] = vector;
}

void MaterialInstance::set_texture(std::string_view name, std::shared_ptr<gl::Texture> texture, int unit) {
    textures[std::string(name)] = std::make_pair(unit, texture);
}
