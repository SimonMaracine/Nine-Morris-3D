#pragma once

#include <string>
#include <vector>
#include <memory>

#include <glm/glm.hpp>

namespace model {
    struct PositionVertex {
        glm::vec3 position;
    };

    struct FullVertex {
        glm::vec3 position;
        glm::vec2 texture_coordinate;
        glm::vec3 normal;
    };

    template<typename V>
    struct Mesh {
        Mesh(const std::vector<V>& vertices, const std::vector<unsigned int>& indices)
            : vertices(vertices), indices(indices) {}

        std::vector<V> vertices;
        std::vector<unsigned int> indices;
    };

    std::shared_ptr<Mesh<FullVertex>> load_model_full(const std::string& file_path);
    std::shared_ptr<Mesh<PositionVertex>> load_model_position(const std::string& file_path);
}

template<typename T>
using Rc = std::shared_ptr<T>;
