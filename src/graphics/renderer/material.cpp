#include <glm/glm.hpp>

#include "graphics/renderer/material.h"
#include "graphics/renderer/opengl/shader.h"
#include "other/logging.h"
#include "other/assert.h"

Material::Material(std::shared_ptr<Shader> shader, int flags)
    : shader(shader), flags(flags) {
    DEB_DEBUG("Created material with shader: {}", shader->get_name());
}

Material::Material(std::shared_ptr<Material> material) {
    shader = material->shader;
    uniforms_mat4 = material->uniforms_mat4;
    uniforms_int = material->uniforms_int;
    uniforms_float = material->uniforms_float;
    uniforms_vec2 = material->uniforms_vec2;
    uniforms_vec3 = material->uniforms_vec3;
    uniforms_vec4 = material->uniforms_vec4;
    textures = material->textures;
    flags = material->flags;
}

Material::~Material() {
    DEB_DEBUG("Deleted material from shader: {}", shader->get_name());
}

void Material::add_variable(UniformType type, std::string_view name) {
    switch (type) {
        case UniformType::Mat4:
            uniforms_mat4[std::string(name)] = glm::mat4(1.0f);
            break;
        case UniformType::Int:
            uniforms_int[std::string(name)] = 0;
            break;
        case UniformType::Float:
            uniforms_float[std::string(name)] = 0.0f;
            break;
        case UniformType::Vec2:
            uniforms_vec2[std::string(name)] = glm::vec2(0.0f);
            break;
        case UniformType::Vec3:
            uniforms_vec3[std::string(name)] = glm::vec3(0.0f);
            break;
        case UniformType::Vec4:
            uniforms_vec4[std::string(name)] = glm::vec4(0.0f);
            break;
        default:
            ASSERT(false, "Unknown uniform type");
            break;
    }
}

void Material::add_texture(std::string_view name) {
    textures[std::string(name)] = std::make_pair(0, nullptr);
}

// --- Material instance

MaterialInstance::MaterialInstance(std::shared_ptr<Material> material)
    : material(std::make_unique<Material>(material)) {
    DEB_DEBUG("Made material instance");
}

MaterialInstance::~MaterialInstance() {
    DEB_DEBUG("Destroyed material instance");
}

std::shared_ptr<MaterialInstance> MaterialInstance::make(std::shared_ptr<Material> material) {
    return std::make_shared<MaterialInstance>(material);
}

void MaterialInstance::bind() {
    material->shader->bind();

    for (const auto& [name, matrix] : material->uniforms_mat4) {
        material->shader->upload_uniform_mat4(name, matrix);
    }

    for (const auto& [name, value] : material->uniforms_int) {
        material->shader->upload_uniform_int(name, value);
    }

    for (const auto& [name, value] : material->uniforms_float) {
        material->shader->upload_uniform_float(name, value);
    }

    for (const auto& [name, vector] : material->uniforms_vec2) {
        material->shader->upload_uniform_vec2(name, vector);
    }

    for (const auto& [name, vector] : material->uniforms_vec3) {
        material->shader->upload_uniform_vec3(name, vector);
    }

    for (const auto& [name, vector] : material->uniforms_vec4) {
        material->shader->upload_uniform_vec4(name, vector);
    }

    // Bind any textures
    for (auto& [name, pair] : material->textures) {
        auto& [unit, texture] = pair;
        material->shader->upload_uniform_int(name, unit);
        texture->bind(unit);
    }
}

void MaterialInstance::set_mat4(std::string_view name, const glm::mat4& matrix) {
    material->uniforms_mat4[std::string(name)] = matrix;
}

void MaterialInstance::set_int(std::string_view name, int value) {
    material->uniforms_int[std::string(name)] = value;
}

void MaterialInstance::set_float(std::string_view name, float value) {
    material->uniforms_float[std::string(name)] = value;
}

void MaterialInstance::set_vec2(std::string_view name, glm::vec2 vector) {
    material->uniforms_vec2[std::string(name)] = vector;
}

void MaterialInstance::set_vec3(std::string_view name, const glm::vec3& vector) {
    material->uniforms_vec3[std::string(name)] = vector;
}

void MaterialInstance::set_vec4(std::string_view name, const glm::vec4& vector) {
    material->uniforms_vec4[std::string(name)] = vector;
}

void MaterialInstance::set_texture(std::string_view name, std::shared_ptr<Texture> texture, int unit) {
    material->textures[std::string(name)] = std::make_pair(unit, texture);
}
