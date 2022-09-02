#pragma once

#include <glm/glm.hpp>
#include <entt/entt.hpp>

#include "nine_morris_3d_engine/graphics/renderer/opengl/shader.h"
#include "nine_morris_3d_engine/graphics/renderer/opengl/texture.h"

class Material {
public:
    enum class UniformType {
        Mat4, Int, Float, Vec2, Vec3, Vec4
    };

    enum {
        // FIXME flags here; anything needed?
    };

    Material(entt::resource_handle<Shader> shader, int flags = 0);
    ~Material();

    void add_uniform(UniformType type, std::string_view name);
    void add_texture(std::string_view name);
private:
    entt::resource_handle<Shader> shader;

    std::unordered_map<std::string, glm::mat4> uniforms_mat4;
    std::unordered_map<std::string, int> uniforms_int;
    std::unordered_map<std::string, float> uniforms_float;
    std::unordered_map<std::string, glm::vec2> uniforms_vec2;
    std::unordered_map<std::string, glm::vec3> uniforms_vec3;
    std::unordered_map<std::string, glm::vec4> uniforms_vec4;

    std::unordered_map<std::string, std::pair<int, entt::resource_handle<Texture>>> textures;

    int flags = 0;  // FIXME is needed?

    friend class MaterialInstance;
};

class MaterialInstance {
public:
    MaterialInstance(entt::resource_handle<Material> material);
    ~MaterialInstance();

    void bind();

    void set_mat4(std::string_view name, const glm::mat4& matrix);
    void set_int(std::string_view name, int value);
    void set_float(std::string_view name, float value);
    void set_vec2(std::string_view name, glm::vec2 vector);
    void set_vec3(std::string_view name, const glm::vec3& vector);
    void set_vec4(std::string_view name, const glm::vec4& vector);
    void set_texture(std::string_view name, entt::resource_handle<Texture> texture, int unit);

    entt::resource_handle<Shader> get_shader() { return shader; }
private:
    entt::resource_handle<Shader> shader;

    std::unordered_map<std::string, glm::mat4> uniforms_mat4;
    std::unordered_map<std::string, int> uniforms_int;
    std::unordered_map<std::string, float> uniforms_float;
    std::unordered_map<std::string, glm::vec2> uniforms_vec2;
    std::unordered_map<std::string, glm::vec3> uniforms_vec3;
    std::unordered_map<std::string, glm::vec4> uniforms_vec4;

    std::unordered_map<std::string, std::pair<int, entt::resource_handle<Texture>>> textures;

    int flags = 0;  // FIXME is needed?
};
