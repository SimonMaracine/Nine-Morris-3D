#include <algorithm>

#include <entt/entt.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "ecs/systems.h"
#include "ecs/components.h"
#include "ecs/game.h"
#include "application/input.h"
#include "opengl/renderer/renderer.h"
#include "other/logging.h"

void systems::load_projection_view(entt::registry& registry, entt::entity camera_entity) {
    auto& camera = CAMERA(camera_entity);

    renderer::load_projection_view(camera.projection_view_matrix);
}

void systems::board_render(entt::registry& registry) {
    auto view = registry.view<TransformComponent,
                              MeshComponent,
                              MaterialComponent,
                              TextureComponent>(entt::exclude<OutlineComponent>);

    for (entt::entity entity : view) {
        auto [transform, mesh, material, textures] = view.get(entity);

        renderer::draw_board(transform.position, transform.rotation, transform.scale,
                             material.shader, mesh.vertex_array, textures.diffuse_map,
                             material.specular_color, material.shininess,
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
        zoom = std::max(zoom, 1.0f);
        zoom = std::min(zoom, 70.0f);

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

void systems::cube_map_render(entt::registry& registry, entt::entity camera_entity) {
    auto& camera = CAMERA(camera_entity);

    auto view = registry.view<SkyboxMeshComponent, SkyboxMaterialComponent,
                              SkyboxTextureComponent>();

    for (entt::entity entity : view) {
        auto [mesh, material, texture] = view.get(entity);

        const glm::mat4& projection_matrix = camera.projection_matrix;
        glm::mat4 view_matrix = glm::mat4(glm::mat3(camera.view_matrix));

        renderer::draw_cube_map(projection_matrix * view_matrix, material.shader,
                                mesh.vertex_array, texture.cube_map);
    }
}

void systems::piece_render(entt::registry& registry, entt::entity hovered_entity) {
    // auto& camera_transform = registry.get<TransformComponent>(camera_entity);

    // auto func = [&camera_transform](const TransformComponent& lhs, const TransformComponent& rhs) {
    //     float distance1 = glm::length(camera_transform.position - lhs.position);
    //     float distance2 = glm::length(camera_transform.position - rhs.position);
    //     return distance1 < distance2;
    // };

    // registry.sort<TransformComponent>(func);

    auto view = registry.view<TransformComponent, MeshComponent,
                              MaterialComponent, TextureComponent,
                              OutlineComponent, PieceComponent>();

    for (entt::entity entity : view) {
        auto [transform, mesh, material, textures, outline, piece] = view.get(entity);

        if (piece.selected) {
            renderer::draw_piece_outline(transform.position, transform.rotation,
                                    transform.scale, material.shader, mesh.vertex_array,
                                    textures.diffuse_map, material.specular_color,
                                    material.shininess, mesh.index_count,
                                    outline.outline_color);
        } else if (piece.show_outline && entity == hovered_entity && piece.active &&
                !piece.pending_remove) {
            renderer::draw_piece_outline(transform.position, transform.rotation,
                                    transform.scale, material.shader, mesh.vertex_array,
                                    textures.diffuse_map, material.specular_color,
                                    material.shininess, mesh.index_count,
                                    glm::vec3(1.0f, 0.5f, 0.0f));
        } else if (piece.to_take && entity == hovered_entity && piece.active) {
            renderer::draw_piece(transform.position, transform.rotation,
                            transform.scale, material.shader, mesh.vertex_array,
                            textures.diffuse_map, material.specular_color,
                            material.shininess, mesh.index_count,
                            glm::vec3(0.9f, 0.0f, 0.0f));
        } else {
            renderer::draw_piece(transform.position, transform.rotation,
                            transform.scale, material.shader, mesh.vertex_array,
                            textures.diffuse_map, material.specular_color,
                            material.shininess, mesh.index_count,
                            glm::vec3(1.0f, 1.0f, 1.0f));
        }
    }
}

void systems::lighting(entt::registry& registry, entt::entity camera_entity) {
    auto& camera_transform = registry.get<TransformComponent>(camera_entity);

    auto view = registry.view<TransformComponent, LightComponent, ShaderComponent>();

    for (entt::entity entity : view) {
        auto [transform, light, shaders] = view.get(entity);

        shaders.board_shader->bind();
        shaders.board_shader->set_uniform_vec3("u_light.position", transform.position);
        shaders.board_shader->set_uniform_vec3("u_light.ambient", light.ambient_color);
        shaders.board_shader->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
        shaders.board_shader->set_uniform_vec3("u_light.specular", light.specular_color);
        shaders.board_shader->set_uniform_vec3("u_view_position", camera_transform.position);

        shaders.piece_shader->bind();
        shaders.piece_shader->set_uniform_vec3("u_light.position", transform.position);
        shaders.piece_shader->set_uniform_vec3("u_light.ambient", light.ambient_color);
        shaders.piece_shader->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
        shaders.piece_shader->set_uniform_vec3("u_light.specular", light.specular_color);
        shaders.piece_shader->set_uniform_vec3("u_view_position", camera_transform.position);
    }
}

void systems::lighting_render(entt::registry& registry, entt::entity camera_entity) {
    auto& camera = CAMERA(camera_entity);

    auto view = registry.view<TransformComponent, LightMeshComponent>();

    for (entt::entity entity : view) {
        auto [transform, light] = view.get(entity);

        light.shader->bind();
        light.shader->set_uniform_matrix("u_projection_matrix", camera.projection_matrix);
        light.shader->set_uniform_matrix("u_view_matrix", camera.view_matrix);

        renderer::draw_light(transform.position);
    }
}

void systems::lighting_move(entt::registry& registry, float dt) {
    auto view = registry.view<TransformComponent, LightMeshComponent>();

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

void systems::origin_render(entt::registry& registry, entt::entity camera_entity) {
    auto& camera = CAMERA(camera_entity);

    auto view = registry.view<OriginComponent>();

    for (entt::entity entity : view) {
        auto& origin = view.get<OriginComponent>(entity);

        origin.shader->bind();
        origin.shader->set_uniform_matrix("u_projection_view_matrix",
                                          camera.projection_view_matrix);

        renderer::draw_origin();
    }
}

void systems::node_render(entt::registry& registry,
                          entt::entity hovered_entity, entt::entity board_entity) {
    auto& state = STATE(board_entity);

    auto view = registry.view<TransformComponent, MeshComponent, NodeMaterialComponent>();

    for (entt::entity entity : view) {
        auto [transform, mesh, material] = view.get(entity);

        if (hovered_entity == entity &&
                state.phase != Phase::None && state.phase != Phase::GameOver) {
            renderer::draw_node(transform.position, transform.scale,
                                material.shader, mesh.vertex_array,
                                glm::vec4(0.7f, 0.7f, 0.7f, 1.0f),
                                mesh.index_count);
        } else {
            renderer::draw_node(transform.position, transform.scale,
                                material.shader, mesh.vertex_array,
                                glm::vec4(0.0f, 0.0f, 0.0f, 0.0f),
                                mesh.index_count);   
        }
    }
}

void systems::render_board_to_depth(entt::registry& registry) {
    auto view = registry.view<TransformComponent,
                              MeshComponent,
                              ShadowComponent>(entt::exclude<PieceComponent>);

    for (entt::entity entity : view) {
        auto [transform, mesh, shadow] = view.get(entity);

        renderer::draw_board_depth(transform.position, transform.rotation,
                                   transform.scale, shadow.shader,
                                   mesh.vertex_array, mesh.index_count);
    }
}

void systems::render_piece_to_depth(entt::registry& registry) {
    auto view = registry.view<TransformComponent,
                              MeshComponent,
                              ShadowComponent>(entt::exclude<GameStateComponent>);

    for (entt::entity entity : view) {
        auto [transform, mesh, shadow] = view.get(entity);

        renderer::draw_piece_depth(transform.position, transform.rotation,
                                   transform.scale, shadow.shader,
                                   mesh.vertex_array, mesh.index_count);
    }
}

// void systems::node_move(entt::registry& registry, float dt) {
//     auto view = registry.view<TransformComponent, NodeMaterialComponent>();

//     for (entt::entity entity : view) {
//         auto& transform = view.get<TransformComponent>(entity);

//         if (input::is_key_pressed(KEY_UP)) {
//             transform.position.z += 0.01f;
//         } else if (input::is_key_pressed(KEY_DOWN)) {
//             transform.position.z -= 0.01f;
//         }

//         if (input::is_key_pressed(KEY_LEFT)) {
//             transform.position.x += 0.01f;
//         } else if (input::is_key_pressed(KEY_RIGHT)) {
//             transform.position.x -= 0.01f;
//         }
//     }
// }
