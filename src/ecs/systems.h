#pragma once

#include <entt/entt.hpp>

namespace systems {
    void render(entt::registry& registry, entt::entity camera_entity);

    void camera(entt::registry& registry, float mouse_wheel, float dx, float dy, float dt);

    void cube_map_render(entt::registry& registry, entt::entity camera_entity);

    void with_outline_render(entt::registry& registry, entt::entity camera_entity,
                             entt::entity hovered_entity);

    void lighting(entt::registry& registry, entt::entity camera_entity);

    void lighting_render(entt::registry& registry, entt::entity camera_entity);

    void lighting_move(entt::registry& registry, float dt);

    void origin_render(entt::registry& registry, entt::entity camera_entity);

    void node_render(entt::registry& registry, entt::entity camera_entity,
                     entt::entity hovered_entity);

    // void node_move(entt::registry& registry, float dt);
}
