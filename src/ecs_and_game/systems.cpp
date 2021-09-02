#include <algorithm>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ecs_and_game/systems.h"
#include "ecs_and_game/components.h"
#include "ecs_and_game/game.h"
#include "application/input.h"
#include "opengl/renderer/renderer.h"
#include "other/logging.h"

void systems::load_projection_view(entt::registry& registry, entt::entity camera_entity) {
    auto& camera = CAMERA(camera_entity);

    renderer::load_projection_view(camera.projection_view_matrix);
}

void systems::board_render(entt::registry& registry) {
    auto view = registry.view<TransformComponent, MeshComponent,
                              MaterialComponent, GameStateComponent>();

    for (entt::entity entity : view) {
        auto [transform, mesh, material] = view.get<TransformComponent, MeshComponent, MaterialComponent>(entity);

        renderer::draw_board(transform.position, transform.rotation, transform.scale,
                             mesh.vertex_array, material.specular_color, material.shininess,
                             mesh.index_count);
    }
}

void systems::board_paint_render(entt::registry& registry) {
    auto view = registry.view<TransformComponent, MeshComponent,
                              MaterialComponent, BoardPaintComponent>();

    for (entt::entity entity : view) {
        auto [transform, mesh, material] = view.get<TransformComponent, MeshComponent, MaterialComponent>(entity);

        renderer::draw_board_paint(transform.position, transform.rotation, transform.scale,
                                   mesh.vertex_array, material.specular_color, material.shininess,
                                   mesh.index_count);
    }
}

void systems::camera(entt::registry& registry, float mouse_wheel, float dx, float dy, float dt) {
    auto view = registry.view<TransformComponent, CameraComponent, CameraMoveComponent>();

    for (entt::entity entity : view) {
        auto [transform, camera, move] = view.get(entity);

        float& pitch = transform.rotation.x;
        float& yaw = transform.rotation.y;
        float& zoom = camera.distance_to_point;

        constexpr float move_speed = 50.0f;
        const float zoom_speed = 1.05f * zoom;

        move.zoom_velocity -= zoom_speed * mouse_wheel * 1.25f * dt;

        if (input::is_key_pressed(KEY_R)) {
            move.zoom_velocity -= zoom_speed * dt;
        } else if (input::is_key_pressed(KEY_F)) {
            move.zoom_velocity += zoom_speed * dt;
        }

        zoom += move.zoom_velocity;

        // Limit zoom
        zoom = std::max(zoom, 5.0f);
        zoom = std::min(zoom, 60.0f);

        if (input::is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)) {
            move.y_velocity -= move_speed * 0.3f * dy * dt;
            move.x_velocity += move_speed * 0.3f * dx * dt;
        }

        if (input::is_key_pressed(KEY_W)) {
            move.y_velocity += move_speed * dt;
        } else if (input::is_key_pressed(KEY_S)) {
            move.y_velocity -= move_speed * dt;
        }

        if (input::is_key_pressed(KEY_A)) {
            move.x_velocity -= move_speed * dt;
        } else if (input::is_key_pressed(KEY_D)) {
            move.x_velocity += move_speed * dt;
        }

        pitch += move.y_velocity;
        camera.angle_around_point += move.x_velocity;

        // Limit pitch
        pitch = std::min(pitch, 90.0f);
        pitch = std::max(pitch, -90.0f);

        // Slow down velocity
        move.x_velocity *= 0.8f;
        move.y_velocity *= 0.8f;
        move.zoom_velocity *= 0.8f;

        float horizontal_distance = zoom * glm::cos(glm::radians(pitch));
        float vertical_distance = zoom * glm::sin(glm::radians(pitch));

        float offset_x =
            horizontal_distance * glm::sin(glm::radians(camera.angle_around_point));
        float offset_z =
            horizontal_distance * glm::cos(glm::radians(camera.angle_around_point));

        transform.position.x = camera.point.x - offset_x;
        transform.position.z = camera.point.z - offset_z;
        transform.position.y = camera.point.y + vertical_distance;

        yaw = 180 - camera.angle_around_point;

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::rotate(matrix, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 negative_camera_position = glm::vec3(-transform.position.x,
                                                       -transform.position.y,
                                                       -transform.position.z);
        matrix = glm::translate(matrix, negative_camera_position);

        camera.view_matrix = matrix;
        camera.projection_view_matrix = camera.projection_matrix * camera.view_matrix;
    }
}

void systems::projection_matrix(entt::registry& registry, float width, float height) {
    auto view = registry.view<CameraComponent>();

    for (entt::entity entity : view) {
        auto& camera = view.get<CameraComponent>(entity);

        camera.projection_matrix = glm::perspective(glm::radians(45.0f), width / height, 0.1f, 70.0f);
        camera.projection_view_matrix = camera.projection_matrix * camera.view_matrix;
    }
}

void systems::skybox_render(entt::registry& registry, entt::entity camera_entity, RenderStorage* storage) {
    auto& camera = CAMERA(camera_entity);

    auto view = registry.view<SkyboxComponent>();

    for (entt::entity _ : view) {
        const glm::mat4& projection_matrix = camera.projection_matrix;
        glm::mat4 view_matrix = glm::mat4(glm::mat3(camera.view_matrix));

        renderer::draw_skybox(projection_matrix * view_matrix);
    }
}

void systems::piece_render(entt::registry& registry, entt::entity hovered_entity,
                           entt::entity camera_entity, RenderStorage* storage) {
    auto& camera_transform = registry.get<TransformComponent>(camera_entity);

    auto func = [&camera_transform](const TransformComponent& lhs, const TransformComponent& rhs) {
        float distance1 = glm::length(camera_transform.position - lhs.position);
        float distance2 = glm::length(camera_transform.position - rhs.position);
        return distance1 > distance2;
    };

    registry.sort<TransformComponent>(func);

    auto view = registry.view<TransformComponent, MeshComponent,
                              MaterialComponent, OutlineComponent,
                              PieceTextureComponent, PieceComponent>(entt::exclude<InactiveTag>);

    view.use<TransformComponent>();

    for (entt::entity entity : view) {
        auto [transform, mesh, material, outline, textures, piece] = view.get(entity);

        if (piece.selected) {
            renderer::draw_piece_outline(transform.position, transform.rotation,
                                    transform.scale, mesh.vertex_array,
                                    textures.diffuse_texture, material.specular_color,
                                    material.shininess, mesh.index_count,
                                    outline.outline_color);
        } else if (piece.show_outline && entity == hovered_entity && piece.in_use && !piece.pending_remove) {
            renderer::draw_piece_outline(transform.position, transform.rotation,
                                    transform.scale, mesh.vertex_array,
                                    textures.diffuse_texture, material.specular_color,
                                    material.shininess, mesh.index_count,
                                    glm::vec3(1.0f, 0.5f, 0.0f));
        } else if (piece.to_take && entity == hovered_entity && piece.in_use) {
            renderer::draw_piece(transform.position, transform.rotation,
                            transform.scale, mesh.vertex_array,
                            textures.diffuse_texture, material.specular_color,
                            material.shininess, mesh.index_count,
                            glm::vec3(0.9f, 0.0f, 0.0f));
        } else {
            renderer::draw_piece(transform.position, transform.rotation,
                            transform.scale, mesh.vertex_array, textures.diffuse_texture,
                            material.specular_color, material.shininess, mesh.index_count,
                            glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }
}

void systems::lighting(entt::registry& registry, entt::entity camera_entity, RenderStorage* storage) {
    auto& camera_transform = registry.get<TransformComponent>(camera_entity);

    auto view = registry.view<TransformComponent, LightComponent>();

    for (entt::entity entity : view) {
        auto [transform, light] = view.get(entity);

        storage->board_shader->bind();
        storage->board_shader->set_uniform_vec3("u_light.position", transform.position);
        storage->board_shader->set_uniform_vec3("u_light.ambient", light.ambient_color);
        storage->board_shader->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
        storage->board_shader->set_uniform_vec3("u_light.specular", light.specular_color);
        storage->board_shader->set_uniform_vec3("u_view_position", camera_transform.position);

        storage->board_paint_shader->bind();
        storage->board_paint_shader->set_uniform_vec3("u_light.position", transform.position);
        storage->board_paint_shader->set_uniform_vec3("u_light.ambient", light.ambient_color);
        storage->board_paint_shader->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
        storage->board_paint_shader->set_uniform_vec3("u_light.specular", light.specular_color);
        storage->board_paint_shader->set_uniform_vec3("u_view_position", camera_transform.position);

        storage->piece_shader->bind();
        storage->piece_shader->set_uniform_vec3("u_light.position", transform.position);
        storage->piece_shader->set_uniform_vec3("u_light.ambient", light.ambient_color);
        storage->piece_shader->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
        storage->piece_shader->set_uniform_vec3("u_light.specular", light.specular_color);
        storage->piece_shader->set_uniform_vec3("u_view_position", camera_transform.position);
    }
}

void systems::lighting_render(entt::registry& registry, entt::entity camera_entity, RenderStorage* storage) {
    auto& camera = CAMERA(camera_entity);

    auto view = registry.view<TransformComponent, QuadTextureComponent>();

    for (entt::entity entity : view) {
        auto [transform, texture] = view.get(entity);

        storage->quad3d_shader->bind();
        storage->quad3d_shader->set_uniform_matrix("u_projection_matrix", camera.projection_matrix);
        storage->quad3d_shader->set_uniform_matrix("u_view_matrix", camera.view_matrix);

        renderer::draw_quad_3d(transform.position, transform.scale, texture.texture);
    }
}

void systems::lighting_move(entt::registry& registry, float dt) {
    auto view = registry.view<TransformComponent, LightComponent>();

    for (entt::entity entity : view) {
        auto& transform = view.get<TransformComponent>(entity);

        if (input::is_key_pressed(KEY_UP)) {
            transform.position.z += 10.0f * dt;
        } else if (input::is_key_pressed(KEY_DOWN)) {
            transform.position.z -= 10.0f * dt;
        }

        if (input::is_key_pressed(KEY_LEFT)) {
            transform.position.x += 10.0f * dt;
        } else if (input::is_key_pressed(KEY_RIGHT)) {
            transform.position.x -= 10.0f * dt;
        }
    }
}

void systems::origin_render(entt::registry& registry, entt::entity camera_entity, RenderStorage* storage) {
    auto& camera = CAMERA(camera_entity);

    auto view = registry.view<OriginComponent>();

    for (entt::entity _ : view) {
        storage->origin_shader->bind();
        storage->origin_shader->set_uniform_matrix("u_projection_view_matrix",
                                                   camera.projection_view_matrix);

        renderer::draw_origin();
    }
}

void systems::node_render(entt::registry& registry, entt::entity hovered_entity, entt::entity board_entity) {
    auto& state = STATE(board_entity);

    auto view = registry.view<TransformComponent, MeshComponent, NodeComponent>();

    for (entt::entity entity : view) {
        auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);

        if (hovered_entity == entity && state.phase != Phase::None && state.phase != Phase::GameOver) {
            renderer::draw_node(transform.position, transform.scale, mesh.vertex_array,
                                glm::vec4(0.7f, 0.7f, 0.7f, 1.0f), mesh.index_count);
        } else {
            renderer::draw_node(transform.position, transform.scale, mesh.vertex_array,
                                glm::vec4(0.0f, 0.0f, 0.0f, 0.0f), mesh.index_count);
        }
    }
}

void systems::render_to_depth(entt::registry& registry) {
    auto view = registry.view<TransformComponent, MeshComponent, ShadowComponent>(entt::exclude<InactiveTag>);

    for (entt::entity entity : view) {
        auto [transform, mesh] = view.get<TransformComponent, MeshComponent>(entity);

        renderer::draw_to_depth(transform.position, transform.rotation,
                                transform.scale, mesh.vertex_array, mesh.index_count);
    }
}

void systems::turn_indicator_render(entt::registry& registry, entt::entity board, RenderStorage* storage) {
    auto& state = STATE(board);

    auto view = registry.view<TransformComponent, TurnIndicatorComponent>();

    for (entt::entity entity : view) {
        auto& transform = view.get<TransformComponent>(entity);

        storage->quad2d_shader->bind();
        storage->quad2d_shader->set_uniform_matrix("u_projection_matrix", storage->orthographic_projection_matrix);

        if (state.turn == Player::White) {
            renderer::draw_quad_2d(transform.position, transform.scale, storage->white_indicator_texture);
        } else {
            renderer::draw_quad_2d(transform.position, transform.scale, storage->black_indicator_texture);
        }
    }
}

void systems::turn_indicator(entt::registry& registry, float width, float height) {
    auto view = registry.view<TransformComponent, TurnIndicatorComponent>();

    for (entt::entity entity : view) {
        auto& transform = view.get<TransformComponent>(entity);

        transform.position = glm::vec3(width - 90, height - 115, 0.0f);
    }
}

// void systems::node_move(entt::registry& registry, float dt, entt::entity cam) {
//     auto& camera = registry.get<CameraComponent>(cam);

//     auto view = registry.view<TransformComponent, NodeComponent>();

//     for (entt::entity entity : view) {
//         auto& transform = view.get<TransformComponent>(entity);

//         if (input::is_key_pressed(KEY_UP)) {
//             transform.position.z += 0.001f;
//         } else if (input::is_key_pressed(KEY_DOWN)) {
//             transform.position.z -= 0.001f;
//         }

//         if (input::is_key_pressed(KEY_LEFT)) {
//             transform.position.x += 0.001f;
//         } else if (input::is_key_pressed(KEY_RIGHT)) {
//             transform.position.x -= 0.001f;
//         }

//         camera.point = transform.position;

//         // SPDLOG_DEBUG("[ {}, {} ]", transform.position.x, transform.position.z);
//     }
// }
