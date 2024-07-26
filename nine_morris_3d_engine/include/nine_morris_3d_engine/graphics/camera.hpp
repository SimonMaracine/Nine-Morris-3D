#pragma once

#include <glm/glm.hpp>

namespace sm {
    struct Camera {
        void set_position_orientation(glm::vec3 position, glm::vec3 at, glm::vec3 up) noexcept;
        void set_position_rotation(glm::vec3 position, glm::vec3 rotation) noexcept;

        void set_projection(int width, int height, float fov, float near, float far) noexcept;
        void set_view(const glm::mat4& view_matrix) noexcept;

        glm::mat4 view_matrix {1.0f};
        glm::mat4 projection_matrix {1.0f};

        // This is a cache
        glm::mat4 projection_view_matrix {1.0f};
    };

    struct Camera2D {
        void set_projection(int left, int right, int bottom, int top) noexcept;

        glm::mat4 projection_matrix {1.0f};
    };
}
