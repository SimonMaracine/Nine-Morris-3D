#pragma once

#include <glm/glm.hpp>

namespace sm {
    struct Camera2D {
        // TODO
        // void set_projection_matrix(int width, int height, float fov, float near, float far);

        glm::mat4 projection_matrix {glm::mat4(1.0f)};
    };
}
