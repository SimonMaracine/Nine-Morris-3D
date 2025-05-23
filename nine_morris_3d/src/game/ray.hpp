#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

glm::vec3 cast_mouse_ray(sm::Ctx& ctx, const sm::Camera3D& camera);
bool ray_aabb_collision(glm::vec3 ray, glm::vec3 origin, const sm::utils::AABB& aabb);
