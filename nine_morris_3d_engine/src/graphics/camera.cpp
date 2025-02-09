#include "nine_morris_3d_engine/graphics/camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace sm {
    void Camera3D::set_position_orientation(glm::vec3 position, glm::vec3 at, glm::vec3 up) {
        m_view_matrix = glm::lookAt(position, at, up);
        m_projection_view_matrix = m_projection_matrix * m_view_matrix;
    }

    void Camera3D::set_position_rotation(glm::vec3 position, glm::vec3 rotation) {
        glm::mat4 matrix {1.0f};
        matrix = glm::translate(matrix, position);
        matrix = glm::rotate(matrix, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

        m_view_matrix = glm::inverse(matrix);
        m_projection_view_matrix = m_projection_matrix * m_view_matrix;
    }

    void Camera3D::set_projection(int width, int height, float fov, float near, float far) {
        m_projection_matrix = glm::perspective(
            glm::radians(fov),
            static_cast<float>(width) / static_cast<float>(height),
            near,
            far
        );
        m_projection_view_matrix = m_projection_matrix * m_view_matrix;
    }

    void Camera3D::set_view(const glm::mat4& view_matrix) {
        m_view_matrix = view_matrix;
        m_projection_view_matrix = m_projection_matrix * m_view_matrix;
    }

    const glm::mat4& Camera3D::view() const {
        return m_view_matrix;
    }

    const glm::mat4& Camera3D::projection() const {
        return m_projection_matrix;
    }

    const glm::mat4& Camera3D::projection_view() const {
        return m_projection_view_matrix;
    }

    void Camera2D::set_projection(int left, int right, int bottom, int top) {
        m_projection_matrix = glm::ortho(
            static_cast<float>(left),
            static_cast<float>(right),
            static_cast<float>(bottom),
            static_cast<float>(top)
        );
    }

    const glm::mat4& Camera2D::projection() const {
        return m_projection_matrix;
    }
}
