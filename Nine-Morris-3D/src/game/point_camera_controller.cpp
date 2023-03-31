#include <engine/engine_application.h>
#include <engine/engine_other.h>

#include "game/point_camera_controller.h"
#include "other/constants.h"

static constexpr float ZOOM_BASE_VELOCITY = 0.03f;
static constexpr float ZOOM_VARIABLE_VELOCITY = 5.0f;
static constexpr float Y_BASE_VELOCITY = 0.03f;
static constexpr float Y_VARIABLE_VELOCITY = 5.0f;
static constexpr float X_BASE_VELOCITY = 0.03f;
static constexpr float X_VARIABLE_VELOCITY = 5.0f;

static constexpr float MIN_DISTANCE_TO_POINT = 5.0f;
static constexpr float MAX_DISTANCE_TO_POINT = 24.0f;

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

PointCameraController::PointCameraController(Camera* camera)
    : CameraController(camera) {
    update_camera(1.0f);
}

PointCameraController::PointCameraController(Camera* camera, int width, int height, float fov, float near,
        float far, const glm::vec3& point, float distance_to_point, float pitch, float sensitivity)
    : CameraController(camera), sensitivity(sensitivity), pitch(pitch), point(point),
      distance_to_point(distance_to_point) {
    camera->set_projection(width, height, fov, near, far);
    update_camera(1.0f);
}

const glm::vec3& PointCameraController::get_position() const {
    return position;
}

const glm::vec3& PointCameraController::get_rotation() const {
    static glm::vec3 rotation = glm::vec3(0.0f);
    rotation.x = pitch;
    rotation.y = yaw;

    return rotation;
}

void PointCameraController::update_controls(float dt) {
    static constexpr float MOVE_SPEED = 2700.0f;
    static constexpr float MOVE_SPEED_MOUSE = MOVE_SPEED * 0.004f;
    static constexpr float ZOOM_SPEED = 370.0f;
    static constexpr float ZOOM_SPEED_WHEEL = ZOOM_SPEED * 0.02f;

    zoom_velocity -= ZOOM_SPEED_WHEEL * mouse_input.mouse_wheel;

    if (input::is_key_pressed(input::Key::R)) {
        zoom_velocity -= ZOOM_SPEED * dt;
    } else if (input::is_key_pressed(input::Key::F)) {
        zoom_velocity += ZOOM_SPEED * dt;
    }

    if (input::is_mouse_button_pressed(input::MouseButton::Right)) {
        y_velocity -= MOVE_SPEED_MOUSE * mouse_input.dy;
        x_velocity += MOVE_SPEED_MOUSE * mouse_input.dx;
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

    mouse_input.mouse_wheel = 0.0f;
    mouse_input.dx = 0.0f;
    mouse_input.dy = 0.0f;
}

void PointCameraController::update_camera(float dt) {
    distance_to_point += zoom_velocity * sensitivity * dt;

    // Calculate automatic distance to point movement
    calculate_auto_distance_to_point(dt);

    // Limit distance_to_point
    distance_to_point = std::max(distance_to_point, MIN_DISTANCE_TO_POINT);
    distance_to_point = std::min(distance_to_point, MAX_DISTANCE_TO_POINT);

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

    camera->set_view(matrix);
}

void PointCameraController::update_friction() {
    // Slow down velocity
    x_velocity *= 0.81f;
    y_velocity *= 0.81f;
    zoom_velocity *= 0.81f;
}

void PointCameraController::set_position(const glm::vec3& position) {
    this->position = position;

    // Calculate distance to point
    distance_to_point = glm::length(position - point);

    // Update camera data
    camera->set_view(position, point - position, UP_VECTOR);

    // Calculate yaw, pitch and angle_around_point
    const glm::vec3 direction = glm::vec3(glm::transpose(camera->get_view_matrix())[2]);
    yaw = glm::degrees(glm::atan(-direction.x, direction.z));
    pitch = glm::degrees(glm::asin(direction.y));
    angle_around_point = 180.0f - yaw;

    // Stop go_towards_position
    movement.auto_move_x = false;
    movement.auto_move_y = false;
    movement.auto_move_zoom = false;
}

void PointCameraController::go_towards_position(const glm::vec3& position) {
    movement.cached_towards_position = position;
    movement.dont_auto_call_go_towards_position = false;

    // Calculate distance to point velocity
    go_towards_position_zoom(position);

    const glm::mat4 view = glm::lookAt(position, point - position, UP_VECTOR);
    const glm::vec3 direction = glm::vec3(glm::transpose(view)[2]);

    // Calculate pitch velocity
    go_towards_position_y(direction);

    // Calculate angle around point velocity
    go_towards_position_x(direction);
}

void PointCameraController::connect_events(Ctx* ctx) {
    ctx->evt.connect<MouseScrolledEvent, &PointCameraController::on_mouse_scrolled>(this);
    ctx->evt.connect<MouseMovedEvent, &PointCameraController::on_mouse_moved>(this);
}

void PointCameraController::disconnect_events(Ctx* ctx) {
    ctx->evt.disconnect(this);
}

void PointCameraController::discard_events(Ctx* ctx) {
    ctx->evt.discard<MouseScrolledEvent>();  // TODO dirty solution
    ctx->evt.discard<MouseMovedEvent>();
}

void PointCameraController::on_mouse_scrolled(const MouseScrolledEvent& event) {
    mouse_input.mouse_wheel = event.scroll;
}

void PointCameraController::on_mouse_moved(const MouseMovedEvent& event) {
    mouse_input.dx = mouse_input.last_mouse_x - event.mouse_x;
    mouse_input.dy = mouse_input.last_mouse_y - event.mouse_y;
    mouse_input.last_mouse_x = event.mouse_x;
    mouse_input.last_mouse_y = event.mouse_y;
}

void PointCameraController::go_towards_position_x(const glm::vec3& direction) {
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

void PointCameraController::go_towards_position_y(const glm::vec3& direction) {
    movement.target_pitch = glm::degrees(glm::asin(direction.y));

    movement.auto_y_velocity = (movement.target_pitch - pitch) * Y_BASE_VELOCITY;
    movement.auto_move_y = true;
    movement.virtual_pitch = pitch;
    y_velocity = 0.0f;
}

void PointCameraController::go_towards_position_zoom(const glm::vec3& position) {
    movement.target_distance_to_point = glm::length(position - point);

    movement.auto_zoom_velocity = (movement.target_distance_to_point - distance_to_point) * ZOOM_BASE_VELOCITY;
    movement.auto_move_zoom = true;
    movement.virtual_distance_to_point = distance_to_point;
    zoom_velocity = 0.0f;
}

void PointCameraController::calculate_auto_angle_around_point(float dt) {
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

void PointCameraController::calculate_auto_pitch(float dt) {
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

void PointCameraController::calculate_auto_distance_to_point(float dt) {
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
