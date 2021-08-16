#pragma once

#include <entt/entt.hpp>

#define CAMERA(entity) registry.get<CameraComponent>(entity)

namespace systems {
    void load_projection_view(entt::registry& registry, entt::entity camera_entity);

    void board_render(entt::registry& registry);

    void camera(entt::registry& registry, float mouse_wheel, float dx, float dy, float dt);

    void cube_map_render(entt::registry& registry, entt::entity camera_entity);

    void piece_render(entt::registry& registry, entt::entity hovered_entity,
                      entt::entity camera_entity);

    void lighting(entt::registry& registry, entt::entity camera_entity);

    void lighting_render(entt::registry& registry, entt::entity camera_entity);

    void lighting_move(entt::registry& registry, float dt);

    void origin_render(entt::registry& registry, entt::entity camera_entity);

    void node_render(entt::registry& registry, entt::entity hovered_entity,
                     entt::entity board_entity);

    void render_board_to_depth(entt::registry& registry);

    void render_piece_to_depth(entt::registry& registry);

    // void node_move(entt::registry& registry, float dt);
}
