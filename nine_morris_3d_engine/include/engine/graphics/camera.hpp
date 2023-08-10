#pragma once

#include <glm/glm.hpp>

namespace sm {
    struct Camera {  // TODO maybe refactor camera using quaternions
        Camera() noexcept = default;
        ~Camera() noexcept = default;

        Camera(const Camera&) noexcept = default;
        Camera& operator=(const Camera&) noexcept = default;
        Camera(Camera&&) noexcept = default;
        Camera& operator=(Camera&&) noexcept = default;

        void set_position_orientation(const glm::vec3& position, const glm::vec3& at, const glm::vec3& up);
        void set_position_rotation(const glm::vec3& position, const glm::vec3& rotation);

        void set_projection_matrix(int width, int height, float fov, float near, float far);
        void set_view_matrix(const glm::mat4& view_matrix);

        glm::mat4 view_matrix = glm::mat4(1.0f);
        glm::mat4 projection_matrix = glm::mat4(1.0f);

        // This is a cache
        glm::mat4 projection_view_matrix = glm::mat4(1.0f);
    };
}
