#include "game/ray.hpp"

#include <nine_morris_3d_engine/external/glm.h++>

glm::vec3 cast_mouse_ray(sm::Ctx& ctx, const sm::Camera& camera) {
    const auto [mouse_x, mouse_y] {ctx.get_mouse_position()};

    // To normalized device space
    const float x {mouse_x / static_cast<float>(ctx.get_window_width()) * 2.0f - 1.0f};
    const float y {mouse_y / static_cast<float>(ctx.get_window_height()) * 2.0f - 1.0f};

    // To homogeneous clip space
    glm::vec4 vector {x, -y, -1.0f, 1.0f};

    // To eye space
    vector = glm::inverse(camera.projection()) * vector;
    vector.z = -1.0f;
    vector.w = 0.0f;

    // To world space
    vector = glm::inverse(camera.view()) * vector;

    return glm::normalize(glm::vec3(vector));
}
