#pragma once

#include <memory>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/buffer.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"

struct TransformComponent {
    TransformComponent() = default;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    float scale = 1.0f;
};

struct CameraComponent {
    CameraComponent(const glm::mat4& projection_matrix, const glm::vec3& point,
                    float distance_to_point)
            : projection_matrix(projection_matrix), point(point),
              distance_to_point(distance_to_point) {}

    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 projection_matrix;
    glm::mat4 projection_view_matrix = glm::mat4(1.0f);  // This is a cache
    glm::vec3 point;
    float distance_to_point;
    float angle_around_point = 0.0f;
    // Pitch and yaw are in transform.rotation
};

struct CameraMoveComponent {
    CameraMoveComponent() = default;

    float x_velocity = 0.0f;
    float y_velocity = 0.0f;
    float zoom_velocity = 0.0f;
};

struct MeshComponent {
    MeshComponent(std::shared_ptr<VertexArray> vertex_array, int index_count)
            : vertex_array(vertex_array), index_count(index_count) {}

    std::shared_ptr<VertexArray> vertex_array;
    int index_count;
};

struct MaterialComponent {
    MaterialComponent(std::shared_ptr<Shader> shader, const glm::vec3& specular_color,
                      float shininess)
            : shader(shader), specular_color(specular_color), shininess(shininess) {}

    std::shared_ptr<Shader> shader;
    glm::vec3 specular_color;
    float shininess;
};

struct TextureComponent {  // TODO maybe I should move this to MaterialComponent
    TextureComponent(std::shared_ptr<Texture> diffuse_map)
            : diffuse_map(diffuse_map) {}
    
    std::shared_ptr<Texture> diffuse_map;
    // std::shared_ptr<Texture> specular_map;
    // std::shared_ptr<Texture> normal_map;
};

struct SkyboxMeshComponent {
    SkyboxMeshComponent(std::shared_ptr<VertexArray> vertex_array)
            : vertex_array(vertex_array) {}

    std::shared_ptr<VertexArray> vertex_array;
};

struct SkyboxMaterialComponent {
    SkyboxMaterialComponent(std::shared_ptr<Shader> shader)
            : shader(shader) {}

    std::shared_ptr<Shader> shader;
};

struct SkyboxTextureComponent {
    SkyboxTextureComponent(std::shared_ptr<Texture3D> cube_map)
            : cube_map(cube_map) {}
    
    std::shared_ptr<Texture3D> cube_map;
};

struct OutlineComponent {
    OutlineComponent(std::shared_ptr<Shader> shader, const glm::vec3& outline_color)
            : shader(shader), outline_color(outline_color) {}

    std::shared_ptr<Shader> shader;
    glm::vec3 outline_color;
};

struct LightComponent {
    LightComponent(const glm::vec3& ambient_color, const glm::vec3& diffuse_color,
                   const glm::vec3& specular_color)
            : ambient_color(ambient_color), diffuse_color(diffuse_color),
              specular_color(specular_color) {}

    glm::vec3 ambient_color;
    glm::vec3 diffuse_color;
    glm::vec3 specular_color;
};

struct ShaderComponent {
    ShaderComponent(std::shared_ptr<Shader> board_shader, std::shared_ptr<Shader> piece_shader)
            : board_shader(board_shader), piece_shader(piece_shader) {}

    std::shared_ptr<Shader> board_shader;
    std::shared_ptr<Shader> piece_shader;
};

struct OriginComponent {
    OriginComponent(std::shared_ptr<Shader> shader) : shader(shader) {}

    std::shared_ptr<Shader> shader;
};

struct LightMeshComponent {
    LightMeshComponent(std::shared_ptr<Shader> shader) : shader(shader) {}

    std::shared_ptr<Shader> shader;
};

struct NodeMaterialComponent {
    NodeMaterialComponent(std::shared_ptr<Shader> shader) : shader(shader) {}

    std::shared_ptr<Shader> shader;
};

struct ShadowComponent {
    ShadowComponent(std::shared_ptr<Shader> shader) : shader(shader) {}

    std::shared_ptr<Shader> shader;
};
