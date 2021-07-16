#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "opengl/renderer/renderer.h"
#include "other/logging.h"

struct InputData {
    int mouse_x;
    int mouse_y;
    int mouse_wheel;
    bool left_mouse_pressed;
    bool right_mouse_pressed;
    float mouse_dt_x;
    float mouse_dt_y;
    bool pressed_A;
    bool pressed_D;
    bool pressed_W;
    bool pressed_S;
    bool pressed_R;
    bool pressed_F;
};

void render_system(entt::registry& registry, entt::entity camera_entity) {
    auto& camera = registry.get<CameraComponent>(camera_entity);

    auto view = registry.view<TransformComponent,
                              MeshComponent,
                              MaterialComponent,
                              TextureComponent>(entt::exclude<OutlineComponent>);

    for (entt::entity entity : view) {
        auto [transform, mesh, material, textures] =
            view.get<TransformComponent, MeshComponent,
                     MaterialComponent, TextureComponent>(entity);

        material.shader->bind();
        material.shader->set_uniform_matrix("u_projection_view_matrix",
                                            camera.projection_view_matrix);
        renderer::draw_model(transform.position, transform.rotation, transform.scale,
                             material.shader, mesh.vertex_array, textures.diffuse_map,
                             material.specular_color, material.shininess,
                             mesh.index_count);
    }
}

void camera_system(entt::registry& registry, const InputData& input, float dt) {
    auto view = registry.view<TransformComponent, CameraComponent>();

    for (entt::entity entity : view) {
        auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

        float& pitch = transform.rotation.x;
        float& yaw = transform.rotation.y;
        float& zoom = camera.distance_to_point;

        const float move_speed = 2.0f;
        const float zoom_speed = 0.08f * zoom;

        zoom += zoom_speed * input.mouse_wheel;
        
        if (input.pressed_R) {
            zoom -= zoom_speed;
        } else if (input.pressed_F) {
            zoom += zoom_speed;
        }

        // Limit zoom
        zoom = std::max(zoom, 1.0f);
        zoom = std::min(zoom, 70.0f);

        if (input.right_mouse_pressed) {
            pitch -= input.mouse_dt_y;
            camera.angle_around_point -= input.mouse_dt_x;
        }

        if (input.pressed_W) {
            pitch += move_speed;
        } else if (input.pressed_S) {
            pitch -= move_speed;
        }

        if (input.pressed_A) {
            camera.angle_around_point -= move_speed;
        } else if (input.pressed_D) {
            camera.angle_around_point += move_speed;
        }

        // Limit pitch
        pitch = std::min(pitch, 90.0f);
        pitch = std::max(pitch, -90.0f);

        float horizontal_distance =
            zoom * glm::cos(glm::radians(pitch));
        float vertical_distance =
            zoom * glm::sin(glm::radians(pitch));

        float offset_x =
            horizontal_distance * glm::sin(glm::radians(camera.angle_around_point));
        float offset_z =
            horizontal_distance * glm::cos(glm::radians(camera.angle_around_point));

        transform.position.x = camera.point.x - offset_x;
        transform.position.z = camera.point.z - offset_z;
        transform.position.y = camera.point.y + vertical_distance;
        
        yaw = 180 - camera.angle_around_point;

        glm::mat4 matrix = glm::mat4(1.0f);
        matrix = glm::rotate(matrix, glm::radians(pitch),
                             glm::vec3(1.0f, 0.0f, 0.0f));
        matrix = glm::rotate(matrix, glm::radians(yaw),
                             glm::vec3(0.0f, 1.0f, 0.0f));
        glm::vec3 negative_camera_position = glm::vec3(-transform.position.x,
                                                       -transform.position.y,
                                                       -transform.position.z);
        matrix = glm::translate(matrix, negative_camera_position);

        camera.view_matrix = matrix;
        camera.projection_view_matrix = camera.projection_matrix * camera.view_matrix;
    }
}

void cube_map_render_system(entt::registry& registry, entt::entity camera_entity) {
    auto& camera = registry.get<CameraComponent>(camera_entity);

    auto view = registry.view<SkyboxMeshComponent, SkyboxMaterialComponent,
                              SkyboxTextureComponent>();

    for (entt::entity entity : view) {
        auto [mesh, material, texture] = view.get<SkyboxMeshComponent,
                                                  SkyboxMaterialComponent,
                                                  SkyboxTextureComponent>(entity);

        const glm::mat4& projection_matrix = camera.projection_matrix;
        glm::mat4 view_matrix = glm::mat4(glm::mat3(camera.view_matrix));

        renderer::draw_cube_map(projection_matrix * view_matrix, material.shader,
                                mesh.vertex_array, texture.cube_map);
    }
}

void pieces_render_system(entt::registry& registry, entt::entity camera_entity,
                          entt::entity hovered_entity) {
    auto& camera = registry.get<CameraComponent>(camera_entity);
    auto& camera_transform = registry.get<TransformComponent>(camera_entity);

    auto view = registry.view<TransformComponent, MeshComponent,
                              MaterialComponent, TextureComponent, OutlineComponent>();

    for (entt::entity entity : view) {
        auto [transform, mesh, material, textures, outline] =
            view.get<TransformComponent, MeshComponent, MaterialComponent,
                     TextureComponent, OutlineComponent>(entity);

        if (hovered_entity == entity) {
            float outline_size = (camera_transform.position - transform.position).length() * 1.2f;

            outline.shader->bind();
            outline.shader->set_uniform_matrix("u_projection_view_matrix",
                                               camera.projection_view_matrix);
            renderer::draw_model_outline(transform.position, transform.rotation,
                                     transform.scale, material.shader, mesh.vertex_array,
                                     textures.diffuse_map, material.specular_color,
                                     material.shininess, mesh.index_count,
                                     outline.outline_color, outline_size);
        } else {
            material.shader->bind();
            material.shader->set_uniform_matrix("u_projection_view_matrix",
                                                camera.projection_view_matrix);
            renderer::draw_model(transform.position, transform.rotation,
                                 transform.scale, material.shader, mesh.vertex_array,
                                 textures.diffuse_map, material.specular_color,
                                 material.shininess, mesh.index_count);
        }
    }
}

void lighting_system(entt::registry& registry, entt::entity camera_entity) {
    auto& camera_transform = registry.get<TransformComponent>(camera_entity);

    auto view = registry.view<TransformComponent, LightComponent, ShaderComponent>();

    for (entt::entity entity : view) {
        auto [transform, light, shader] = view.get<TransformComponent, LightComponent,
                                                   ShaderComponent>(entity);

        shader.shader->bind();
        shader.shader->set_uniform_vec3("u_light.position", transform.position);
        shader.shader->set_uniform_vec3("u_light.ambient", light.ambient_color);
        shader.shader->set_uniform_vec3("u_light.diffuse", light.diffuse_color);
        shader.shader->set_uniform_vec3("u_light.specular", light.specular_color);
        
        shader.shader->set_uniform_vec3("u_view_position", camera_transform.position);
    }
}

void origin_render_system(entt::registry& registry, entt::entity camera_entity) {
    auto& camera = registry.get<CameraComponent>(camera_entity);

    auto view = registry.view<OriginComponent>();

    for (entt::entity entity : view) {
        OriginComponent& origin = view.get<OriginComponent>(entity);

        origin.shader->bind();
        origin.shader->set_uniform_matrix("u_projection_view_matrix",
                                          camera.projection_view_matrix);

        renderer::draw_origin();
    }
}
