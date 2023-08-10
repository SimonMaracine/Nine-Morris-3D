#pragma once

#include <glm/glm.hpp>

namespace sm {
    struct Camera2D {
        Camera2D() noexcept = default;
        ~Camera2D() noexcept = default;

        Camera2D(const Camera2D&) noexcept = default;
        Camera2D& operator=(const Camera2D&) noexcept = default;
        Camera2D(Camera2D&&) noexcept = default;
        Camera2D& operator=(Camera2D&&) noexcept = default;

        // TODO
        // void set_projection_matrix(int width, int height, float fov, float near, float far);

        glm::mat4 projection_matrix = glm::mat4(1.0f);
    };
}
