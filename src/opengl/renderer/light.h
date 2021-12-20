#pragma once

#include <glm/glm.hpp>

class Light {
public:
    Light() = default;
    Light(const glm::vec3& position, const glm::vec3& ambient_color, const glm::vec3& diffuse_color,
            const glm::vec3& specular_color)
        : position(position), ambient_color(ambient_color), diffuse_color(diffuse_color),
          specular_color(specular_color) {}
    ~Light() = default;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 ambient_color = glm::vec3(0.0f);
    glm::vec3 diffuse_color = glm::vec3(0.0f);
    glm::vec3 specular_color = glm::vec3(0.0f);
};
