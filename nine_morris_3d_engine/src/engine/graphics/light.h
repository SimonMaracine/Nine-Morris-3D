#pragma once

#include <glm/glm.hpp>

namespace sm {
    struct DirectionalLight {
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 ambient_color = glm::vec3(0.0f);
        glm::vec3 diffuse_color = glm::vec3(0.0f);
        glm::vec3 specular_color = glm::vec3(0.0f);
    };
}
