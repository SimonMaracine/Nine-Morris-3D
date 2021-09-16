#pragma once

#include <entt/entt.hpp>

#include "opengl/renderer/renderer.h"

#define CAMERA(entity) registry.get<CameraComponent>(entity)

typedef const renderer::Storage RenderStorage;

namespace systems {
    void load_projection_view(entt::registry& registry, entt::entity camera_entity);
    void board_render(entt::registry& registry);
    void board_paint_render(entt::registry& registry);
    void camera(entt::registry& registry, float mouse_wheel, float dx, float dy, float dt);
    void projection_matrix(entt::registry& registry, float width, float height);
    void skybox_render(entt::registry& registry, entt::entity camera_entity, RenderStorage* storage);
    void piece_render(entt::registry& registry, entt::entity hovered_entity,
                      entt::entity camera_entity, RenderStorage* storage);
    void lighting(entt::registry& registry, entt::entity camera_entity, RenderStorage* storage);
#ifndef NDEBUG
    void lighting_render(entt::registry& registry, entt::entity camera_entity, RenderStorage* storage);
    void lighting_move(entt::registry& registry, float dt);
    void origin_render(entt::registry& registry, entt::entity camera_entity, RenderStorage* storage);
#endif
    void node_render(entt::registry& registry, entt::entity hovered_entity, entt::entity board_entity);
    void render_to_depth(entt::registry& registry);
    void turn_indicator_render(entt::registry& registry, entt::entity board, RenderStorage* storage);
    void turn_indicator(entt::registry& registry, float witdh, float height);

    // void node_move(entt::registry& registry, float dt, entt::entity cam);
}
