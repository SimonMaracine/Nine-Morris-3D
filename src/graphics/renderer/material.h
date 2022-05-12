#pragma once

#include <memory>
#include <string_view>
#include <unordered_map>
#include <utility>

#include <glm/glm.hpp>

#include "graphics/renderer/opengl/shader.h"
#include "graphics/renderer/opengl/texture.h"

class Material {
public:
    enum class UniformType {
        Mat4, Int, Vec2, Vec3, Vec4, Float
    };

    enum {
        Hoverable = 1 << 0
    };

    Material(std::shared_ptr<Shader> shader, int flags = 0);
    Material(std::shared_ptr<Material> material);
    ~Material();

    void add_variable(UniformType type, std::string_view name);
    void add_texture(std::string_view name);
private:
    std::shared_ptr<Shader> shader;

    std::unordered_map<std::string, glm::mat4> uniforms_mat4;
    std::unordered_map<std::string, int> uniforms_int;
    std::unordered_map<std::string, float> uniforms_float;
    std::unordered_map<std::string, glm::vec2> uniforms_vec2;
    std::unordered_map<std::string, glm::vec3> uniforms_vec3;
    std::unordered_map<std::string, glm::vec4> uniforms_vec4;

    std::unordered_map<std::string, std::pair<int, std::shared_ptr<Texture>>> textures;

    int flags = 0;

    friend class MaterialInstance;
};

class MaterialInstance {
public:
    MaterialInstance(std::shared_ptr<Material> material);
    ~MaterialInstance();

    static std::shared_ptr<MaterialInstance> make(std::shared_ptr<Material> material);

    void bind();

    void set_mat4(std::string_view name, const glm::mat4& matrix);
    void set_int(std::string_view name, int value);
    void set_float(std::string_view name, float value);
    void set_vec2(std::string_view name, glm::vec2 vector);
    void set_vec3(std::string_view name, const glm::vec3& vector);
    void set_vec4(std::string_view name, const glm::vec4& vector);
    void set_texture(std::string_view name, std::shared_ptr<Texture> texture, int unit);

    std::shared_ptr<Shader> get_shader() { return material->shader; }

    bool is_hoverable() { return material->flags & static_cast<int>(Material::Hoverable); }
private:
    std::unique_ptr<Material> material;
};
