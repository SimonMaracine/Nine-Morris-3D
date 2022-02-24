#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <utility>

#include <glm/glm.hpp>

#include "graphics/renderer/shader.h"
#include "graphics/renderer/texture.h"

class Material {
public:
    enum class UniformType {
        Mat4, Int, Vec2, Vec3, Vec4, Float
    };

    Material(std::shared_ptr<Shader> shader);
    Material(std::shared_ptr<Material> material);
    ~Material();

    void add_variable(UniformType type, const std::string& name);
    void add_texture(const std::string& name, std::shared_ptr<Texture> texture, int unit);
private:
    std::shared_ptr<Shader> shader;

    std::unordered_map<std::string, glm::mat4> uniforms_mat4;
    std::unordered_map<std::string, int> uniforms_int;
    std::unordered_map<std::string, float> uniforms_float;
    std::unordered_map<std::string, glm::vec2> uniforms_vec2;
    std::unordered_map<std::string, glm::vec3> uniforms_vec3;
    std::unordered_map<std::string, glm::vec4> uniforms_vec4;

    std::unordered_map<std::string, std::pair<int, std::shared_ptr<Texture>>> textures;

    friend class MaterialInstance;
};

class MaterialInstance {
public:
    MaterialInstance(std::shared_ptr<Material> material);
    ~MaterialInstance();

    static std::shared_ptr<MaterialInstance> make(std::shared_ptr<Material> material);

    void bind();

    void set_mat4(const std::string& name, const glm::mat4& matrix);
    void set_int(const std::string& name, int value);
    void set_float(const std::string& name, float value);
    void set_vec2(const std::string& name, const glm::vec2& vector);
    void set_vec3(const std::string& name, const glm::vec3& vector);
    void set_vec4(const std::string& name, const glm::vec4& vector);

    std::shared_ptr<Shader> get_shader() { return material->shader; }
private:
    std::unique_ptr<Material> material;
};
