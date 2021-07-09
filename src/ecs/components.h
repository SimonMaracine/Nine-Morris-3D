#include <memory>
#include <unordered_map>
#include <string>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/vertex_buffer.h"
#include "opengl/renderer/shader.h"
#include "opengl/renderer/texture.h"

struct TransformComponent {
    TransformComponent(glm::vec3 position, glm::vec3 rotation, glm::vec3 scale)
            : position(position), rotation(rotation), scale(scale) {}

    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec3 scale;
};

struct CameraComponent {
    CameraComponent(glm::mat3 projection_matrix)
            : projection_matrix(projection_matrix) {}

    glm::mat3 view_matrix = glm::mat3(1.0f);
    glm::mat3 projection_matrix;
    glm::mat3 view_projection_matrix = glm::mat3(1.0f);  // This is a cache
};

struct MeshComponent {
    MeshComponent(std::shared_ptr<VertexArray> vertex_array,
                  std::shared_ptr<VertexBuffer> vertex_buffer)
            : vertex_array(vertex_array), vertex_buffer(vertex_buffer) {}

    std::shared_ptr<VertexArray> vertex_array;
    std::shared_ptr<VertexBuffer> vertex_buffer;
};

struct MaterialComponent {
    MaterialComponent(std::shared_ptr<Shader> shader,
                      std::unordered_map<std::string, int> uniforms)
            : shader(shader), uniforms(uniforms) {}

    std::shared_ptr<Shader> shader;
    std::unordered_map<std::string, int> uniforms;
};

struct TextureComponent {
    TextureComponent(std::shared_ptr<Texture> albedo_map)
            : albedo_map(albedo_map) {}
    
    std::shared_ptr<Texture> albedo_map;
    // std::shared_ptr<Texture> specular_map;
    // std::shared_ptr<Texture> normal_map;
};
