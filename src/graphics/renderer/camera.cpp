#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/renderer/camera.h"
#include "application/input.h"
#include "other/logging.h"

Camera::Camera(float pitch, const glm::vec3& point, float distance_to_point, const glm::mat4& projection_matrix)
    : pitch(pitch), point(point), distance_to_point(distance_to_point), projection_matrix(projection_matrix) {

}

void Camera::update(float mouse_wheel, float dx, float dy, float dt) {
    float& zoom = distance_to_point;

    constexpr float MOVE_SPEED = 50.0f;
    constexpr float MOVE_SPEED_MOUSE = MOVE_SPEED * 0.3f;
    constexpr float ZOOM_SPEED = 9.0f;
    constexpr float ZOOM_SPEED_WHEEL = ZOOM_SPEED * 1.25f;

    zoom_velocity -= ZOOM_SPEED_WHEEL * mouse_wheel;

    if (input::is_key_pressed(KEY_R)) {
        zoom_velocity -= ZOOM_SPEED;
    } else if (input::is_key_pressed(KEY_F)) {
        zoom_velocity += ZOOM_SPEED;
    }

    zoom += zoom_velocity * dt;

    // Limit zoom
    zoom = std::max(zoom, 5.0f);
    zoom = std::min(zoom, 30.0f);

    if (input::is_mouse_button_pressed(MOUSE_BUTTON_RIGHT)) {
        y_velocity -= MOVE_SPEED_MOUSE * dy;
        x_velocity += MOVE_SPEED_MOUSE * dx;
    }

    if (input::is_key_pressed(KEY_W)) {
        y_velocity += MOVE_SPEED;
    } else if (input::is_key_pressed(KEY_S)) {
        y_velocity -= MOVE_SPEED;
    }

    if (input::is_key_pressed(KEY_A)) {
        x_velocity -= MOVE_SPEED;
    } else if (input::is_key_pressed(KEY_D)) {
        x_velocity += MOVE_SPEED;
    }

    pitch += y_velocity * dt;
    angle_around_point += x_velocity * dt;

    // Limit pitch
    pitch = std::min(pitch, 90.0f);
    pitch = std::max(pitch, -90.0f);

    const float horizontal_distance = zoom * glm::cos(glm::radians(pitch));
    const float vertical_distance = zoom * glm::sin(glm::radians(pitch));

    const float offset_x = horizontal_distance * glm::sin(glm::radians(angle_around_point));
    const float offset_z = horizontal_distance * glm::cos(glm::radians(angle_around_point));

    position.x = point.x - offset_x;
    position.z = point.z - offset_z;
    position.y = point.y + vertical_distance;

    yaw = 180.0f - angle_around_point;

    // Calculate view matrix and update projection-view matrix
    // based on pitch, yaw and position calculated above
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::rotate(matrix, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::translate(matrix, -position);

    view_matrix = matrix;
    projection_view_matrix = projection_matrix * view_matrix;
}

void Camera::update_friction() {
    // Slow down velocity
    x_velocity *= 0.8f;
    y_velocity *= 0.8f;
    zoom_velocity *= 0.8f;
}

void Camera::update_projection(float width, float height) {
    projection_matrix = glm::perspective(glm::radians(FOV), width / height, NEAR, FAR);
    projection_view_matrix = projection_matrix * view_matrix;
}
