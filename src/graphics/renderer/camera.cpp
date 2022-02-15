#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/renderer/camera.h"
#include "application/input.h"
#include "other/logging.h"

constexpr float ZOOM_BASE_VELOCITY = 0.09f;
constexpr float ZOOM_VARIABLE_VELOCITY = 6.0f;
constexpr float Y_BASE_VELOCITY = 0.09f;
constexpr float Y_VARIABLE_VELOCITY = 6.0f;
constexpr float X_BASE_VELOCITY = 0.09f;
constexpr float X_VARIABLE_VELOCITY = 6.0f;

Camera::Camera(float sensitivity, float pitch, const glm::vec3& point, float distance_to_point,
        const glm::mat4& projection_matrix)
    : sensitivity(sensitivity), pitch(pitch), point(point), distance_to_point(distance_to_point),
      projection_matrix(projection_matrix) {
    update(0.0f, 0.0f, 0.0f, 0.0f);
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

    zoom += zoom_velocity * sensitivity * dt;

    if (auto_move_zoom) {
        zoom += auto_zoom_velocity * dt + (target_distance_to_point - distance_to_point)
                * ZOOM_VARIABLE_VELOCITY * dt;
    }
    if (auto_move_zoom && glm::abs(target_distance_to_point - distance_to_point) < 0.05f) {
        distance_to_point = target_distance_to_point;
        auto_zoom_velocity = 0.0f;
        auto_move_zoom = false;
    }

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

    pitch += y_velocity * sensitivity * dt;
    angle_around_point += x_velocity * sensitivity * dt;

    // if (auto_move_y) {
    //     pitch += auto_y_velocity * dt + (target_pitch - pitch) * Y_VARIABLE_VELOCITY * dt;
    // }
    // if (auto_move_y && glm::abs(target_pitch - pitch) < 0.05f) {
    //     pitch = target_pitch;
    //     auto_y_velocity = 0.0f;
    //     auto_move_y = false;
    // }

    if (auto_move_x) {
        angle_around_point += auto_x_velocity * dt + (target_angle_around_point - angle_around_point)
                * X_VARIABLE_VELOCITY * dt;
    }
    if (auto_move_x && glm::abs(target_angle_around_point - angle_around_point) < 0.05f) {
        angle_around_point = target_angle_around_point;
        auto_x_velocity = 0.0f;
        auto_move_x = false;
    }

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

void Camera::set_position(const glm::vec3& position) {
    this->position = position;

    // Calculate distance to point
    distance_to_point = glm::length(position - point);

    // Calculate matrices
    view_matrix = glm::lookAt(position, point - position, glm::vec3(0.0f, 1.0f, 0.0f));
    projection_view_matrix = projection_matrix * view_matrix;

    // Calculate yaw, pitch and angle_around_point
    const glm::vec3 direction = glm::vec3(glm::transpose(view_matrix)[2]);
    yaw = glm::degrees(glm::atan(-direction.x, direction.z));
    pitch = glm::degrees(glm::asin(direction.y));
    angle_around_point = 180.0f - yaw;
}

void Camera::go_towards_position(const glm::vec3& position) {
    // target_distance_to_point = glm::length(position - point);
    // auto_zoom_velocity = (target_distance_to_point - distance_to_point) * ZOOM_BASE_VELOCITY;
    // auto_move_zoom = true;

    const glm::mat4 view = glm::lookAt(position, point - position, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::vec3 direction = glm::vec3(glm::transpose(view)[2]);

    // target_pitch = glm::degrees(glm::asin(direction.y));
    // auto_y_velocity = (target_pitch - pitch) * Y_BASE_VELOCITY;
    // auto_move_y = true;

    target_angle_around_point = static_cast<int>(180.0f - glm::degrees(glm::atan(-direction.x, direction.z))) % 360;
    if (target_angle_around_point < 0.0f) {
        target_angle_around_point += 360.0f;
    }
    auto_x_velocity = (target_angle_around_point - angle_around_point) * X_BASE_VELOCITY;
    auto_move_x = true;
}
