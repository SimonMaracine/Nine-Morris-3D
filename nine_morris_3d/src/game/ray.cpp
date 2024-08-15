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

// https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms

bool ray_aabb_collision(glm::vec3 ray, glm::vec3 origin, const sm::utils::AABB& aabb) {
    const float dir_x {1.0f / ray.x};
    const float dir_y {1.0f / ray.y};
    const float dir_z {1.0f / ray.z};

    const float t1 {(aabb.min.x - origin.x) * dir_x};
    const float t2 {(aabb.max.x - origin.x) * dir_x};
    const float t3 {(aabb.min.y - origin.y) * dir_y};
    const float t4 {(aabb.max.y - origin.y) * dir_y};
    const float t5 {(aabb.min.z - origin.z) * dir_z};
    const float t6 {(aabb.max.z - origin.z) * dir_z};

    const float tmin {glm::max(glm::max(glm::min(t1, t2), glm::min(t3, t4)), glm::min(t5, t6))};
    const float tmax {glm::min(glm::min(glm::max(t1, t2), glm::max(t3, t4)), glm::max(t5, t6))};

    if (tmax < 0) {
        return false;
    }

    if (tmin > tmax) {
        return false;
    }

    return true;
}
