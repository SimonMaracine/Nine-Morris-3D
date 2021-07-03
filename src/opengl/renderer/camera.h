#pragma once

#include <glm/glm.hpp>

class PerspectiveCamera {
public:
    PerspectiveCamera(float left, float right, float bottom, float top);
    ~PerspectiveCamera() = default;

    void update();
private:
    void recalculate_view_matrix();

    glm::mat3 view_matrix = glm::mat3(1.0f);
    glm::mat3 projection_matrix = glm::mat3(1.0f);
    glm::mat3 view_projection_matrix = glm::mat3(1.0f);

    glm::vec3 position;
    glm::vec3 rotation;  // In degrees
};
