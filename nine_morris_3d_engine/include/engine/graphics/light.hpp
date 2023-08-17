#pragma once

#include <glm/glm.hpp>

namespace sm {
    struct DirectionalLight {
        glm::vec3 position {};
        glm::vec3 ambient_color {};  // Usually quite a low value
        glm::vec3 diffuse_color {};  // Usually just 1.0f
        glm::vec3 specular_color {};  // Usually either 0.0f or 1.0f
    };
}
