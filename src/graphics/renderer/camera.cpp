#include <algorithm>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "graphics/renderer/camera.h"
#include "application/input.h"
#include "other/logging.h"

constexpr float ZOOM_BASE_VELOCITY = 0.03f;
constexpr float ZOOM_VARIABLE_VELOCITY = 5.0f;
constexpr float Y_BASE_VELOCITY = 0.03f;
constexpr float Y_VARIABLE_VELOCITY = 5.0f;
constexpr float X_BASE_VELOCITY = 0.03f;
constexpr float X_VARIABLE_VELOCITY = 5.0f;

static float calculate_angle_velocity(float target_angle, float angle) {
    float integer_angle;
    const float fract = glm::modf(angle, integer_angle);
    float result = target_angle - static_cast<float>(static_cast<int>(integer_angle) % 360) - fract;

    if (result > 0.0f) {
        if (result > 180.0f) {
            result = -(360.0f - result);
        }
    } else {
        if (-result > 180.0f) {
            result = 360.0f + result;
        }
    }

    return result;
}

Camera::Camera(float sensitivity, float pitch, const glm::vec3& point, float distance_to_point,
        const glm::mat4& projection_matrix)
    : sensitivity(sensitivity), pitch(pitch), point(point), distance_to_point(distance_to_point),
      projection_matrix(projection_matrix) {
    update(0.0f, 0.0f, 0.0f, 0.0f);
}

void Camera::update(float mouse_wheel, float dx, float dy, float dt) {
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

    distance_to_point += zoom_velocity * sensitivity * dt;

    // Calculate automatic distance to point movement
    calculate_auto_distance_to_point(dt);

    // Limit distance_to_point
    distance_to_point = std::max(distance_to_point, 5.0f);
    distance_to_point = std::min(distance_to_point, 30.0f);

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

    // Calculate automatic pitch movement
    calculate_auto_pitch(dt);

    // Calculate automatic angle around point movement
    calculate_auto_angle_around_point(dt);

    // Limit pitch
    pitch = std::min(pitch, 90.0f);
    pitch = std::max(pitch, -90.0f);

    const float horizontal_distance = distance_to_point * glm::cos(glm::radians(pitch));
    const float vertical_distance = distance_to_point * glm::sin(glm::radians(pitch));

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
    cached_towards_position = position;
    dont_auto_call_go_towards_position = false;

    // Calculate distance to point velocity
    go_towards_position_zoom(position);

    const glm::mat4 view = glm::lookAt(position, point - position, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::vec3 direction = glm::vec3(glm::transpose(view)[2]);

    // Calculate pitch velocity
    go_towards_position_y(position, direction);

    // Calculate angle around point velocity
    go_towards_position_x(position, direction);
}

void Camera::go_towards_position_x(const glm::vec3& position, const glm::vec3& direction) {
    float integer_angle;
    const float fract = glm::modf(180.0f - glm::degrees(glm::atan(-direction.x, direction.z)), integer_angle);
    target_angle_around_point = static_cast<float>(static_cast<int>(integer_angle) % 360) - fract;
    if (target_angle_around_point < 0.0f) {
        target_angle_around_point += 360.0f;
    }
    const float angle = calculate_angle_velocity(target_angle_around_point, angle_around_point);
    auto_x_velocity = angle * X_BASE_VELOCITY;
    auto_move_x = true;
    virtual_angle_around_point = angle_around_point;
    x_velocity = 0.0f;
}

void Camera::go_towards_position_y(const glm::vec3& position, const glm::vec3& direction) {
    target_pitch = glm::degrees(glm::asin(direction.y));
    auto_y_velocity = (target_pitch - pitch) * Y_BASE_VELOCITY;
    auto_move_y = true;
    virtual_pitch = pitch;
    y_velocity = 0.0f;
}

void Camera::go_towards_position_zoom(const glm::vec3& position) {
    target_distance_to_point = glm::length(position - point);
    auto_zoom_velocity = (target_distance_to_point - distance_to_point) * ZOOM_BASE_VELOCITY;
    auto_move_zoom = true;
    virtual_distance_to_point = distance_to_point;
    zoom_velocity = 0.0f;
}

void Camera::calculate_auto_angle_around_point(float dt) {
    if (x_velocity < -0.1f || x_velocity > 0.1f) {
        auto_move_x = false;
        dont_auto_call_go_towards_position = true;
    }
    if (auto_move_x) {
        const float angle = calculate_angle_velocity(target_angle_around_point, virtual_angle_around_point);
        const float delta = auto_x_velocity * dt + angle * X_VARIABLE_VELOCITY * dt;
        angle_around_point += delta;
        virtual_angle_around_point += delta;
    }
    const float angle = calculate_angle_velocity(target_angle_around_point, virtual_angle_around_point);
    if (auto_move_x && glm::abs(angle) < 0.02f) {
        angle_around_point = target_angle_around_point;
        auto_move_x = false;
    } else if (auto_move_x && glm::abs(angle) < 1.0f) {
        if (!dont_auto_call_go_towards_position) {
            go_towards_position(cached_towards_position);
        }
    }
}

void Camera::calculate_auto_pitch(float dt) {
    if (y_velocity < -0.1f || y_velocity > 0.1f) {
        auto_move_y = false;
        dont_auto_call_go_towards_position = true;
    }
    if (auto_move_y) {
        const float delta = auto_y_velocity * dt + (target_pitch - virtual_pitch) * Y_VARIABLE_VELOCITY * dt;
        pitch += delta;
        virtual_pitch += delta;
    }
    if (auto_move_y && glm::abs(target_pitch - virtual_pitch) < 0.02f) {
        pitch = target_pitch;
        auto_move_y = false;
    } else if (auto_move_y && glm::abs(target_pitch - virtual_pitch) < 1.0f) {
        if (!dont_auto_call_go_towards_position) {
            go_towards_position(cached_towards_position);
        }
    }
}

void Camera::calculate_auto_distance_to_point(float dt) {
    if (zoom_velocity < -0.1f || zoom_velocity > 0.1f) {
        auto_move_zoom = false;
        dont_auto_call_go_towards_position = true;
    }
    if (auto_move_zoom) {
        const float delta = auto_zoom_velocity * dt + (target_distance_to_point - virtual_distance_to_point)
                * ZOOM_VARIABLE_VELOCITY * dt;
        distance_to_point += delta;
        virtual_distance_to_point += delta;
    }
    if (auto_move_zoom && glm::abs(target_distance_to_point - virtual_distance_to_point) < 0.02f) {
        distance_to_point = target_distance_to_point;
        auto_move_zoom = false;
    } else if (auto_move_zoom && glm::abs(target_distance_to_point - virtual_distance_to_point) < 1.0f) {
        if (!dont_auto_call_go_towards_position) {
            go_towards_position(cached_towards_position);
        }
    }
}
