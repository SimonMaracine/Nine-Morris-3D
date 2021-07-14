#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <vector>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/vertex_buffer.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"

struct TransformComponent {
    TransformComponent() = default;
    TransformComponent(const glm::vec3& rotation) : rotation(rotation) {}
    TransformComponent(const glm::vec3& position, const glm::vec3& rotation, float scale)
            : position(position), rotation(rotation), scale(scale) {}

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

struct MeshComponent {
    MeshComponent(std::shared_ptr<VertexArray> vertex_array, int index_count)
            : vertex_array(vertex_array), index_count(index_count) {}

    std::shared_ptr<VertexArray> vertex_array;
    int index_count;
};

struct MaterialComponent {
    MaterialComponent(std::shared_ptr<Shader> shader,
                      std::unordered_map<std::string, int> uniforms)
            : shader(shader), uniforms(uniforms) {}

    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, int> uniforms;
};

struct TextureComponent {
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
