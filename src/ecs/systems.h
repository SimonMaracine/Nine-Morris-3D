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
    // bool A_pressed = false;
    // bool D_pressed = false;
    // bool W_pressed = false;
    // bool S_pressed = false;
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

void camera_system(entt::registry& registry, const InputData& input) {
    auto view = registry.view<TransformComponent, CameraComponent>();

    for (entt::entity entity : view) {
        auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

        float& pitch = transform.rotation.x;
        float& yaw = transform.rotation.y;
        float& zoom = camera.distance_to_point;

        zoom += input.mouse_wheel;

        if (zoom < 0.0f) {
            zoom = 0.0f;
        } else if (zoom > 70.0f) {
            zoom = 70.0f;
        }

        if (input.right_mouse_pressed) {
            pitch -= input.mouse_dt_y;
            
            if (pitch > 90.0f) {
                pitch = 90.0f;
            } else if (pitch < -90.0f) {
                pitch = -90.0f;
            }

            camera.angle_around_point -= input.mouse_dt_x;
        }

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

    auto view = registry.view<TransformComponent, MeshComponent,
                              MaterialComponent, TextureComponent, OutlineComponent>();

    for (entt::entity entity : view) {
        auto [transform, mesh, material, textures, outline] =
            view.get<TransformComponent, MeshComponent,
                     MaterialComponent, TextureComponent,
                     OutlineComponent>(entity);

        if (hovered_entity == entity) {
            outline.shader->bind();
            outline.shader->set_uniform_matrix("u_projection_view_matrix",
                                               camera.projection_view_matrix);
            renderer::draw_model_outline(transform.position, transform.rotation,
                                     transform.scale, material.shader, mesh.vertex_array,
                                     textures.diffuse_map, material.specular_color,
                                     material.shininess, mesh.index_count,
                                     outline.outline_color);
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

    auto view = registry.view<TransformComponent, OriginComponent>();

    for (entt::entity entity : view) {
        auto [transform, origin] = view.get<TransformComponent, OriginComponent>(entity);

        origin.shader->bind();
        origin.shader->set_uniform_matrix("u_projection_view_matrix",
                                          camera.projection_view_matrix);

        renderer::draw_origin();
    }
}
