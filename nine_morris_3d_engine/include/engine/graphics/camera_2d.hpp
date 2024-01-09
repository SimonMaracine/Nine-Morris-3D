#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace sm {
    struct Camera2D {
        void set_projection(int left, int right, int bottom, int top) {
            projection_matrix = glm::ortho(
                static_cast<float>(left),
                static_cast<float>(right),
                static_cast<float>(bottom),
                static_cast<float>(top)
            );
        }

        glm::mat4 projection_matrix {1.0f};
    };
}
