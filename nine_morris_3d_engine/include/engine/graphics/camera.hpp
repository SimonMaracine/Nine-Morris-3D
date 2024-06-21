#pragma once

#include <glm/glm.hpp>

namespace sm {
    struct Camera {
        void set_position_orientation(const glm::vec3& position, const glm::vec3& at, const glm::vec3& up);
        void set_position_rotation(const glm::vec3& position, const glm::vec3& rotation);

        void set_projection(int width, int height, float fov, float near, float far);
        void set_view(const glm::mat4& view_matrix);

        glm::mat4 view_matrix {1.0f};
        glm::mat4 projection_matrix {1.0f};

        // This is a cache
        glm::mat4 projection_view_matrix {1.0f};
    };

    struct Camera2D {
        void set_projection(int left, int right, int bottom, int top);

        glm::mat4 projection_matrix {1.0f};
    };
}
