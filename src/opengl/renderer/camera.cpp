#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "opengl/renderer/camera.h"
#include "application/input.h"

Camera::Camera(float pitch, const glm::mat4& projection_matrix, const glm::vec3& point, float distance_to_point)
    : pitch(pitch), projection_matrix(projection_matrix), point(point), distance_to_point(distance_to_point) {

}

void Camera::update(float mouse_wheel, float dx, float dy, float dt) {
    float& zoom = distance_to_point;

    constexpr float move_speed = 50.0f;
    const float zoom_speed = 1.05f * zoom;

    zoom_velocity -= zoom_speed * mouse_wheel * 1.25f * dt;

    if (input::is_key_pressed(KEY_R)) {
        zoom_velocity -= zoom_speed * dt;
    } else if (input::is_key_pressed(KEY_F)) {
        zoom_velocity += zoom_speed * dt;
    }

    zoom += zoom_velocity;

    // Limit zoom
    zoom = std::max(zoom, 5.0f);
    zoom = std::min(zoom, 30.0f);

    if (input::is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)) {
        y_velocity -= move_speed * 0.3f * dy * dt;
        x_velocity += move_speed * 0.3f * dx * dt;
    }

    if (input::is_key_pressed(KEY_W)) {
        y_velocity += move_speed * dt;
    } else if (input::is_key_pressed(KEY_S)) {
        y_velocity -= move_speed * dt;
    }

    if (input::is_key_pressed(KEY_A)) {
        x_velocity -= move_speed * dt;
    } else if (input::is_key_pressed(KEY_D)) {
        x_velocity += move_speed * dt;
    }

    pitch += y_velocity;
    angle_around_point += x_velocity;

    // Limit pitch
    pitch = std::min(pitch, 90.0f);
    pitch = std::max(pitch, -90.0f);

    // Slow down velocity
    x_velocity *= 0.8f;
    y_velocity *= 0.8f;
    zoom_velocity *= 0.8f;

    float horizontal_distance = zoom * glm::cos(glm::radians(pitch));
    float vertical_distance = zoom * glm::sin(glm::radians(pitch));

    float offset_x = horizontal_distance * glm::sin(glm::radians(angle_around_point));
    float offset_z = horizontal_distance * glm::cos(glm::radians(angle_around_point));

    position.x = point.x - offset_x;
    position.z = point.z - offset_z;
    position.y = point.y + vertical_distance;

    yaw = 180.0f - angle_around_point;

    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::rotate(matrix, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 negative_camera_position = glm::vec3(-position.x, -position.y, -position.z);
    matrix = glm::translate(matrix, negative_camera_position);

    view_matrix = matrix;
    projection_view_matrix = projection_matrix * view_matrix;
}

void Camera::update_projection(float width, float height) {
    projection_matrix = glm::perspective(glm::radians(FOV), width / height, NEAR, FAR);
    projection_view_matrix = projection_matrix * view_matrix;
}
