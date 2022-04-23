#pragma once

#include <glm/glm.hpp>

/**
 * This represents a directional light in the scene.
 */
struct DirectionalLight {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 ambient_color = glm::vec3(0.0f);
    glm::vec3 diffuse_color = glm::vec3(0.0f);
    glm::vec3 specular_color = glm::vec3(0.0f);
};
