#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

namespace model {
    struct VertexP {
        glm::vec3 position = glm::vec3(0.0f);
    };

    struct Vertex {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec2 texture_coordinate = glm::vec2(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
    };

    template<typename V>
    struct Mesh {
        Mesh(const std::vector<V>& vertices, const std::vector<unsigned int>& indices)
            : vertices(vertices), indices(indices) {}

        std::vector<V> vertices;
        std::vector<unsigned int> indices;
    };

    std::shared_ptr<Mesh<Vertex>> load_model(const std::string& file_path);
    std::shared_ptr<Mesh<VertexP>> load_model_position(const std::string& file_path);
}
