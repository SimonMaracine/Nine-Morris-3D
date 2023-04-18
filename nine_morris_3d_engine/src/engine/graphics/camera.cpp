#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "engine/graphics/camera.h"

namespace sm {
    void Camera::set_projection(int width, int height, float fov, float near, float far) {
        projection_matrix = glm::perspective(
            glm::radians(fov), static_cast<float>(width) / static_cast<float>(height), near, far
        );
        projection_view_matrix = projection_matrix * view_matrix;
    }

    void Camera::set_view(const glm::vec3& position, const glm::vec3& at, const glm::vec3& up) {
        view_matrix = glm::lookAt(position, at, up);
        projection_view_matrix = projection_matrix * view_matrix;
    }

    void Camera::set_view(const glm::mat4& view_matrix) {
        this->view_matrix = view_matrix;
        projection_view_matrix = projection_matrix * view_matrix;
    }

    void Camera::set_position_rotation(const glm::vec3& position, const glm::vec3& rotation) {
        recalculate_view(position, rotation);
    }

    void Camera::recalculate_view(const glm::vec3& position, const glm::vec3& rotation) {
        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        view_matrix = glm::inverse(matrix);
        projection_view_matrix = projection_matrix * view_matrix;
    }
}
