#pragma once

#include <string>
#include <vector>
#include <utility>

#include <glm/glm.hpp>

namespace model {
    enum class Model {
        Board, WhitePiece, BlackPiece, Node
    };

    struct Vertex {
        glm::vec3 position;
        glm::vec2 texture_coordinate;
        glm::vec3 normal;
    };

    struct Mesh {
        Model name;
        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;
    };

    std::tuple<Mesh, Mesh, Mesh, Mesh> load_model(const std::string& file_path);
}
