#pragma once

#include <glm/glm.hpp>

#include "engine/other/encrypt.h"

namespace mesh {
    struct PTN {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec2 texture_coordinate = glm::vec2(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
    };

    struct P {
        glm::vec3 position = glm::vec3(0.0f);
    };

    struct PTNT {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec2 texture_coordinate = glm::vec2(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        glm::vec3 tangent = glm::vec3(0.0f);
    };

    template<typename V>
    struct Mesh {
        Mesh(const std::vector<V>& vertices, const std::vector<unsigned int>& indices)
            : vertices(vertices), indices(indices) {}
        ~Mesh() = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) = delete;
        Mesh& operator=(Mesh&&) = delete;

        std::vector<V> vertices;
        std::vector<unsigned int> indices;
    };

    std::shared_ptr<Mesh<PTN>> load_model_PTN(std::string_view file_path, bool flip_winding = false);
    std::shared_ptr<Mesh<PTN>> load_model_PTN(encrypt::EncryptedFile file_path, bool flip_winding = false);

    std::shared_ptr<Mesh<P>> load_model_P(std::string_view file_path, bool flip_winding = false);
    std::shared_ptr<Mesh<P>> load_model_P(encrypt::EncryptedFile file_path, bool flip_winding = false);

    std::shared_ptr<Mesh<PTNT>> load_model_PTNT(std::string_view file_path, bool flip_winding = false);
    std::shared_ptr<Mesh<PTNT>> load_model_PTNT(encrypt::EncryptedFile file_path, bool flip_winding = false);
}
