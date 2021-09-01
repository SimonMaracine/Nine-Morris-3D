#pragma once

#include <string>
#include <vector>

#include <glm/glm.hpp>

namespace model {
    struct Vertex {
        glm::vec3 position;
        glm::vec2 texture_coordinate;
        glm::vec3 normal;
    };

    struct Mesh {
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };

    Mesh load_model(const std::string& file_path);
}
