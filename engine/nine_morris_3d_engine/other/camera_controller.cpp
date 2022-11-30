#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "nine_morris_3d_engine/application/input.h"
#include "nine_morris_3d_engine/graphics/camera.h"
#include "nine_morris_3d_engine/other/camera_controller.h"

static constexpr float ZOOM_BASE_VELOCITY = 0.03f;
static constexpr float ZOOM_VARIABLE_VELOCITY = 5.0f;
static constexpr float Y_BASE_VELOCITY = 0.03f;
static constexpr float Y_VARIABLE_VELOCITY = 5.0f;
static constexpr float X_BASE_VELOCITY = 0.03f;
static constexpr float X_VARIABLE_VELOCITY = 5.0f;

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

CameraController::CameraController(Camera* camera)
    : camera(camera) {
    update(0.0f, 0.0f, 0.0f, 1.0f);
}

CameraController::CameraController(Camera* camera, int width, int height, float fov, float near, float far,
        const glm::vec3& point, float distance_to_point, float pitch, float sensitivity)
    : sensitivity(sensitivity), camera(camera), pitch(pitch), point(point), distance_to_point(distance_to_point) {
    camera->set_projection(width, height, fov, near, far);
    update(0.0f, 0.0f, 0.0f, 1.0f);
}

void CameraController::update(float mouse_wheel, float dx, float dy, float dt) {
    constexpr float MOVE_SPEED = 3200.0f;
    constexpr float MOVE_SPEED_MOUSE = MOVE_SPEED * 0.0039f;
    constexpr float ZOOM_SPEED = 576.0f;
    constexpr float ZOOM_SPEED_WHEEL = ZOOM_SPEED * 0.01953f;

    zoom_velocity -= ZOOM_SPEED_WHEEL * mouse_wheel;

    if (input::is_key_pressed(input::Key::R)) {
        zoom_velocity -= ZOOM_SPEED * dt;
    } else if (input::is_key_pressed(input::Key::F)) {
        zoom_velocity += ZOOM_SPEED * dt;
    }

    if (input::is_mouse_button_pressed(input::MouseButton::RIGHT)) {
        y_velocity -= MOVE_SPEED_MOUSE * dy;
        x_velocity += MOVE_SPEED_MOUSE * dx;
    }

    if (input::is_key_pressed(input::Key::W)) {
        y_velocity += MOVE_SPEED * dt;
    } else if (input::is_key_pressed(input::Key::S)) {
        y_velocity -= MOVE_SPEED * dt;
    }

    if (input::is_key_pressed(input::Key::A)) {
        x_velocity -= MOVE_SPEED * dt;
    } else if (input::is_key_pressed(input::Key::D)) {
        x_velocity += MOVE_SPEED * dt;
    }

    distance_to_point += zoom_velocity * sensitivity * dt;

    // Calculate automatic distance to point movement
    calculate_auto_distance_to_point(dt);

    // Limit distance_to_point
    distance_to_point = std::max(distance_to_point, 5.0f);
    distance_to_point = std::min(distance_to_point, 30.0f);

    pitch += y_velocity * sensitivity * dt;

    // Calculate automatic pitch movement
    calculate_auto_pitch(dt);

    // Limit pitch
    pitch = std::min(pitch, 90.0f);
    pitch = std::max(pitch, -90.0f);

    angle_around_point += x_velocity * sensitivity * dt;

    // Calculate automatic angle around point movement
    calculate_auto_angle_around_point(dt);

    const float horizontal_distance = distance_to_point * glm::cos(glm::radians(pitch));
    const float vertical_distance = distance_to_point * glm::sin(glm::radians(pitch));

    const float offset_x = horizontal_distance * glm::sin(glm::radians(angle_around_point));
    const float offset_z = horizontal_distance * glm::cos(glm::radians(angle_around_point));

    position.x = point.x - offset_x;
    position.z = point.z - offset_z;
    position.y = point.y + vertical_distance;

    yaw = 180.0f - angle_around_point;

    // Update camera data
    glm::mat4 matrix = glm::mat4(1.0f);
    matrix = glm::rotate(matrix, glm::radians(pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::translate(matrix, -position);

    camera->set_view(matrix, position, glm::vec3(pitch, yaw, 0.0f));
}

void CameraController::update_friction() {
    // Slow down velocity
    x_velocity *= 0.8f;
    y_velocity *= 0.8f;
    zoom_velocity *= 0.8f;
}

void CameraController::set_position(const glm::vec3& position) {
    this->position = position;

    // Calculate distance to point
    distance_to_point = glm::length(position - point);

    // Update camera data
    camera->set_view(position, point - position, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(pitch, yaw, 0.0f));

    // Calculate yaw, pitch and angle_around_point
    const glm::vec3 direction = glm::vec3(glm::transpose(camera->get_view_matrix())[2]);
    yaw = glm::degrees(glm::atan(-direction.x, direction.z));
    pitch = glm::degrees(glm::asin(direction.y));
    angle_around_point = 180.0f - yaw;

    camera->set_rotation(glm::vec3(pitch, yaw, 0.0f));  // FIXME redundant calculations

    // Stop go_towards_position
    movement.auto_move_x = false;
    movement.auto_move_y = false;
    movement.auto_move_zoom = false;
}

void CameraController::go_towards_position(const glm::vec3& position) {
    movement.cached_towards_position = position;
    movement.dont_auto_call_go_towards_position = false;

    // Calculate distance to point velocity
    go_towards_position_zoom(position);

    const glm::mat4 view = glm::lookAt(position, point - position, glm::vec3(0.0f, 1.0f, 0.0f));
    const glm::vec3 direction = glm::vec3(glm::transpose(view)[2]);

    // Calculate pitch velocity
    go_towards_position_y(direction);

    // Calculate angle around point velocity
    go_towards_position_x(direction);
}

void CameraController::go_towards_position_x(const glm::vec3& direction) {
    float integer_angle;
    const float fract = glm::modf(180.0f - glm::degrees(glm::atan(-direction.x, direction.z)), integer_angle);

    movement.target_angle_around_point = static_cast<float>(static_cast<int>(integer_angle) % 360) - fract;

    if (movement.target_angle_around_point < 0.0f) {
        movement.target_angle_around_point += 360.0f;
    }

    const float angle = calculate_angle_velocity(movement.target_angle_around_point, angle_around_point);

    movement.auto_x_velocity = angle * X_BASE_VELOCITY;
    movement.auto_move_x = true;
    movement.virtual_angle_around_point = angle_around_point;
    x_velocity = 0.0f;
}

void CameraController::go_towards_position_y(const glm::vec3& direction) {
    movement.target_pitch = glm::degrees(glm::asin(direction.y));

    movement.auto_y_velocity = (movement.target_pitch - pitch) * Y_BASE_VELOCITY;
    movement.auto_move_y = true;
    movement.virtual_pitch = pitch;
    y_velocity = 0.0f;
}

void CameraController::go_towards_position_zoom(const glm::vec3& position) {
    movement.target_distance_to_point = glm::length(position - point);

    movement.auto_zoom_velocity = (movement.target_distance_to_point - distance_to_point) * ZOOM_BASE_VELOCITY;
    movement.auto_move_zoom = true;
    movement.virtual_distance_to_point = distance_to_point;
    zoom_velocity = 0.0f;
}

void CameraController::calculate_auto_angle_around_point(float dt) {
    if (x_velocity < -0.1f || x_velocity > 0.1f) {
        movement.auto_move_x = false;
        movement.dont_auto_call_go_towards_position = true;
    }

    if (movement.auto_move_x) {
        const float angle = calculate_angle_velocity(
            movement.target_angle_around_point, movement.virtual_angle_around_point
        );
        const float delta = movement.auto_x_velocity * dt + angle * X_VARIABLE_VELOCITY * dt;
        angle_around_point += delta;
        movement.virtual_angle_around_point += delta;
    }

    const float angle = calculate_angle_velocity(
        movement.target_angle_around_point, movement.virtual_angle_around_point
    );

    if (movement.auto_move_x && glm::abs(angle) < 0.02f) {
        angle_around_point = movement.target_angle_around_point;
        movement.auto_move_x = false;
    } else if (movement.auto_move_x && glm::abs(angle) < 1.0f) {
        if (!movement.dont_auto_call_go_towards_position) {
            go_towards_position(movement.cached_towards_position);
        }
    }
}

void CameraController::calculate_auto_pitch(float dt) {
    if (y_velocity < -0.1f || y_velocity > 0.1f) {
        movement.auto_move_y = false;
        movement.dont_auto_call_go_towards_position = true;
    }

    if (movement.auto_move_y) {
        const float delta = (
            movement.auto_y_velocity * dt + (movement.target_pitch - movement.virtual_pitch) * Y_VARIABLE_VELOCITY * dt
        );
        pitch += delta;
        movement.virtual_pitch += delta;
    }

    if (movement.auto_move_y && glm::abs(movement.target_pitch - movement.virtual_pitch) < 0.02f) {
        pitch = movement.target_pitch;
        movement.auto_move_y = false;
    } else if (movement.auto_move_y && glm::abs(movement.target_pitch - movement.virtual_pitch) < 1.0f) {
        if (!movement.dont_auto_call_go_towards_position) {
            go_towards_position(movement.cached_towards_position);
        }
    }
}

void CameraController::calculate_auto_distance_to_point(float dt) {
    if (zoom_velocity < -0.1f || zoom_velocity > 0.1f) {
        movement.auto_move_zoom = false;
        movement.dont_auto_call_go_towards_position = true;
    }

    if (movement.auto_move_zoom) {
        const float delta = (
            movement.auto_zoom_velocity * dt + (movement.target_distance_to_point - movement.virtual_distance_to_point)
            * ZOOM_VARIABLE_VELOCITY * dt
        );
        distance_to_point += delta;
        movement.virtual_distance_to_point += delta;
    }

    if (movement.auto_move_zoom && glm::abs(movement.target_distance_to_point - movement.virtual_distance_to_point) < 0.02f) {
        distance_to_point = movement.target_distance_to_point;
        movement.auto_move_zoom = false;
    } else if (movement.auto_move_zoom && glm::abs(movement.target_distance_to_point - movement.virtual_distance_to_point) < 1.0f) {
        if (!movement.dont_auto_call_go_towards_position) {
            go_towards_position(movement.cached_towards_position);
        }
    }
}
