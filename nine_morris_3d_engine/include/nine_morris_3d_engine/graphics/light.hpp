#pragma once

#include <glm/glm.hpp>

namespace sm {
    struct DirectionalLight {
        glm::vec3 direction {};  // Never set both x and z to 0.0
        glm::vec3 ambient_color {};  // Usually quite a low value
        glm::vec3 diffuse_color {};  // Usually just 1.0
        glm::vec3 specular_color {};  // Usually either 0.0 or 1.0
    };

    struct PointLight {
        glm::vec3 position {};
        glm::vec3 ambient_color {};
        glm::vec3 diffuse_color {};
        glm::vec3 specular_color {};

        float falloff_linear {0.07f};
        float falloff_quadratic {0.017f};
    };

    /*
        Distance Constant Linear Quadratic
        7        1.0      0.7    1.8 - not useful
        13       1.0      0.35   0.44
        20       1.0      0.22   0.20
        32       1.0      0.14   0.07
        50       1.0      0.09   0.032
        65       1.0      0.07   0.017
        100      1.0      0.045  0.0075
        160      1.0      0.027  0.0028
        200      1.0      0.022  0.0019
        325      1.0      0.014  0.0007
        600      1.0      0.007  0.0002
        3250     1.0      0.0014 0.000007 - not useful
    */

    struct ShadowBox {
        float left {};
        float right {};
        float bottom {};
        float top {};
        float near_ {};
        float far_ {};
        glm::vec3 position {};
    };
}
