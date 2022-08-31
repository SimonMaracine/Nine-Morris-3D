#pragma once

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/other/encryption.h"

namespace mesh {
    struct VPTN {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec2 texture_coordinate = glm::vec2(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
    };

    struct VP {
        glm::vec3 position = glm::vec3(0.0f);
    };

    struct VPTNT {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec2 texture_coordinate = glm::vec2(0.0f);
        glm::vec3 normal = glm::vec3(0.0f);
        glm::vec3 tangent = glm::vec3(0.0f);
    };

    template<typename V>
    struct Mesh {
        Mesh(const std::vector<V>& vertices, const std::vector<unsigned int>& indices)
            : vertices(vertices), indices(indices) {}

        std::vector<V> vertices;
        std::vector<unsigned int> indices;
    };

    std::shared_ptr<Mesh<VPTN>> load_model_VPTN(std::string_view file_path, bool flip_winding_order = false);
    std::shared_ptr<Mesh<VPTN>> load_model_VPTN(const encryption::EncryptedFile& file_path, bool flip_winding_order = false);

    std::shared_ptr<Mesh<VP>> load_model_VP(std::string_view file_path, bool flip_winding_order = false);
    std::shared_ptr<Mesh<VP>> load_model_VP(const encryption::EncryptedFile& file_path, bool flip_winding_order = false);

    std::shared_ptr<Mesh<VPTNT>> load_model_VPTNT(std::string_view file_path, bool flip_winding_order = false);
    std::shared_ptr<Mesh<VPTNT>> load_model_VPTNT(const encryption::EncryptedFile& file_path, bool flip_winding_order = false);
}
