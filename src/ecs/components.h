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
    MeshComponent(Rc<VertexArray> vertex_array, int index_count)
        : vertex_array(vertex_array), index_count(index_count) {}

    Rc<VertexArray> vertex_array;
    int index_count;
};

struct MaterialComponent {
    MaterialComponent(Rc<Shader> shader, const glm::vec3& specular_color, float shininess)
        : shader(shader), specular_color(specular_color), shininess(shininess) {}

    Rc<Shader> shader;
    glm::vec3 specular_color;
    float shininess;
};

struct TextureComponent {  // TODO maybe I should move this to MaterialComponent
    TextureComponent(Rc<Texture> diffuse_map)
        : diffuse_map(diffuse_map) {}

    Rc<Texture> diffuse_map;
    // Rc<Texture> specular_map;
    // Rc<Texture> normal_map;
};

struct SkyboxMeshComponent {
    SkyboxMeshComponent(Rc<VertexArray> vertex_array)
        : vertex_array(vertex_array) {}

    Rc<VertexArray> vertex_array;
};

struct SkyboxMaterialComponent {
    SkyboxMaterialComponent(Rc<Shader> shader)
        : shader(shader) {}

    Rc<Shader> shader;
};

struct SkyboxTextureComponent {
    SkyboxTextureComponent(Rc<Texture3D> cube_map)
        : cube_map(cube_map) {}

    Rc<Texture3D> cube_map;
};

struct OutlineComponent {
    OutlineComponent(Rc<Shader> shader, const glm::vec3& outline_color)
        : shader(shader), outline_color(outline_color) {}

    Rc<Shader> shader;
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
    ShaderComponent(Rc<Shader> board_shader, Rc<Shader> piece_shader)
        : board_shader(board_shader), piece_shader(piece_shader) {}

    Rc<Shader> board_shader;
    Rc<Shader> piece_shader;
};

struct OriginComponent {
    OriginComponent(Rc<Shader> shader) : shader(shader) {}

    Rc<Shader> shader;
};

struct LightMeshComponent {
    LightMeshComponent(Rc<Shader> shader) : shader(shader) {}

    Rc<Shader> shader;
};

struct NodeMaterialComponent {
    NodeMaterialComponent(Rc<Shader> shader) : shader(shader) {}

    Rc<Shader> shader;
};

struct ShadowComponent {
    ShadowComponent(Rc<Shader> shader) : shader(shader) {}

    Rc<Shader> shader;
};
