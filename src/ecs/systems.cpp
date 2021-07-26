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

void systems::board_render(entt::registry& registry, entt::entity camera_entity) {
    auto& camera = registry.get<CameraComponent>(camera_entity);

    auto view = registry.view<TransformComponent,
                              MeshComponent,
                              MaterialComponent,
                              TextureComponent>(entt::exclude<OutlineComponent>);

    for (entt::entity entity : view) {
        auto [transform, mesh, material, textures] = view.get(entity);

        material.shader->bind();
        material.shader->set_uniform_matrix("u_projection_view_matrix",
                                            camera.projection_view_matrix);
        renderer::draw_model(transform.position, transform.rotation, transform.scale,
                             material.shader, mesh.vertex_array, textures.diffuse_map,
                             material.specular_color, material.shininess,
                             mesh.index_count, glm::vec3(1.0f, 1.0f, 1.0f));
    }
}

void systems::camera(entt::registry& registry, float mouse_wheel, float dx, float dy, float dt) {
    auto view = registry.view<TransformComponent, CameraComponent>();

    for (entt::entity entity : view) {
        auto [transform, camera] = view.get(entity);

        float& pitch = transform.rotation.x;
        float& yaw = transform.rotation.y;
        float& zoom = camera.distance_to_point;

        constexpr float move_speed = 100.0f;
        const float zoom_speed = 5.0f * zoom;

        zoom -= zoom_speed * mouse_wheel * 0.9f * dt;
        
        if (input::is_key_pressed(KEY_R)) {
            zoom -= zoom_speed * dt;
        } else if (input::is_key_pressed(KEY_F)) {
            zoom += zoom_speed * dt;
        }

        // Limit zoom
        zoom = std::max(zoom, 1.0f);
        zoom = std::min(zoom, 70.0f);

        if (input::is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)) {
            pitch -= move_speed * 0.2f * dy * dt;
            camera.angle_around_point += move_speed * 0.2f * dx * dt;
        }

        if (input::is_key_pressed(KEY_W)) {
            pitch += move_speed * dt;
        } else if (input::is_key_pressed(KEY_S)) {
            pitch -= move_speed * dt;
        }

        if (input::is_key_pressed(KEY_A)) {
            camera.angle_around_point -= move_speed * dt;
        } else if (input::is_key_pressed(KEY_D)) {
            camera.angle_around_point += move_speed * dt;
        }

        // Limit pitch
        pitch = std::min(pitch, 90.0f);
        pitch = std::max(pitch, -90.0f);

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
    auto& camera = registry.get<CameraComponent>(camera_entity);

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

void systems::piece_render(entt::registry& registry, entt::entity camera_entity,
                           entt::entity hovered_entity) {
    auto& camera = registry.get<CameraComponent>(camera_entity);

    auto view = registry.view<TransformComponent, MeshComponent,
                              MaterialComponent, TextureComponent,
                              OutlineComponent, PieceComponent>();

    for (entt::entity entity : view) {
        auto [transform, mesh, material, textures, outline, piece] = view.get(entity);

        if (hovered_entity == entity && piece.active && piece.show_outline) {
            constexpr float outline_size = 3.6f;

            outline.shader->bind();
            outline.shader->set_uniform_matrix("u_projection_view_matrix",
                                               camera.projection_view_matrix);
            renderer::draw_model_outline(transform.position, transform.rotation,
                                    transform.scale, material.shader, mesh.vertex_array,
                                    textures.diffuse_map, material.specular_color,
                                    material.shininess, mesh.index_count,
                                    outline.outline_color, outline_size);
        } else if (hovered_entity == entity && piece.active && piece.to_take) {
            material.shader->bind();
            material.shader->set_uniform_matrix("u_projection_view_matrix",
                                                camera.projection_view_matrix);
            renderer::draw_model(transform.position, transform.rotation,
                            transform.scale, material.shader, mesh.vertex_array,
                            textures.diffuse_map, material.specular_color,
                            material.shininess, mesh.index_count,
                            glm::vec3(0.9f, 0.0f, 0.0f));
        } else {
            material.shader->bind();
            material.shader->set_uniform_matrix("u_projection_view_matrix",
                                                camera.projection_view_matrix);
            renderer::draw_model(transform.position, transform.rotation,
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
        auto [transform, light, shader] = view.get(entity);

        shader.shader->bind();
        shader.shader->set_uniform_vec3("u_light.position", transform.position);
        shader.shader->set_uniform_vec3("u_light.ambient", light.ambient_color);
        shader.shader->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
        shader.shader->set_uniform_vec3("u_light.specular", light.specular_color);
        
        shader.shader->set_uniform_vec3("u_view_position", camera_transform.position);
    }
}

void systems::lighting_render(entt::registry& registry, entt::entity camera_entity) {
    auto& camera = registry.get<CameraComponent>(camera_entity);

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
    auto& camera = registry.get<CameraComponent>(camera_entity);

    auto view = registry.view<OriginComponent>();

    for (entt::entity entity : view) {
        auto& origin = view.get<OriginComponent>(entity);

        origin.shader->bind();
        origin.shader->set_uniform_matrix("u_projection_view_matrix",
                                          camera.projection_view_matrix);

        renderer::draw_origin();
    }
}

void systems::node_render(entt::registry& registry, entt::entity camera_entity,
                        entt::entity hovered_entity) {
    auto& camera = registry.get<CameraComponent>(camera_entity);

    auto view = registry.view<TransformComponent, MeshComponent, NodeMaterialComponent>();

    for (entt::entity entity : view) {
        auto [transform, mesh, material] = view.get(entity);

        material.shader->bind();
        material.shader->set_uniform_matrix("u_projection_view_matrix",
                                            camera.projection_view_matrix);

        if (hovered_entity == entity) {
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
