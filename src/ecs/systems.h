#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "application/input.h"
#include "opengl/renderer/renderer.h"
#include "other/logging.h"

void render_system(entt::registry& registry, entt::entity camera_entity);

void camera_system(entt::registry& registry, float mouse_wheel, float dx, float dy, float dt);

void cube_map_render_system(entt::registry& registry, entt::entity camera_entity);

void with_outline_render_system(entt::registry& registry, entt::entity camera_entity,
                                entt::entity hovered_entity);

void lighting_system(entt::registry& registry, entt::entity camera_entity);

void lighting_render_system(entt::registry& registry, entt::entity camera_entity);

void lighting_move_system(entt::registry& registry, float dt);

void origin_render_system(entt::registry& registry, entt::entity camera_entity);

void node_render_system(entt::registry& registry, entt::entity camera_entity,
                        entt::entity hovered_entity);

// void node_move_system(entt::registry& registry, float dt);
