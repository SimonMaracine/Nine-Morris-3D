#include "point_camera_controller.hpp"

#include <nine_morris_3d_engine/external/glm.h++>

static constexpr float ZOOM_BASE_VELOCITY {0.03f};
static constexpr float ZOOM_VARIABLE_VELOCITY {5.0f};
static constexpr float Y_BASE_VELOCITY {0.03f};
static constexpr float Y_VARIABLE_VELOCITY {5.0f};
static constexpr float X_BASE_VELOCITY {0.03f};
static constexpr float X_VARIABLE_VELOCITY {5.0f};

static constexpr float MIN_DISTANCE_TO_POINT {6.0f};
static constexpr float MAX_DISTANCE_TO_POINT {30.0f};

static constexpr float FRICTION {0.81f};

static float calculate_angle_velocity(float target_angle, float angle) {
    float integer_angle;
    const float fract {glm::modf(angle, integer_angle)};
    float result {target_angle - static_cast<float>(static_cast<int>(integer_angle) % 360) - fract};

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

PointCameraController::PointCameraController(sm::Camera& camera)
    : sm::CameraController(camera) {
    update_camera(1.0f);
}

PointCameraController::PointCameraController(
    sm::Camera& camera,
    int width,
    int height,
    glm::vec3 point,
    float distance_to_point,
    float pitch,
    float sensitivity
)
    : sm::CameraController(camera), sensitivity(sensitivity), m_pitch(pitch), m_point(point),
    m_distance_to_point(distance_to_point) {
    camera.set_projection(width, height, LENS_FOV, LENS_NEAR, LENS_FAR);
    update_camera(1.0f);
}

glm::vec3 PointCameraController::get_position() const noexcept {
    return m_position;
}

glm::vec3 PointCameraController::get_rotation() const noexcept {
    return glm::vec3(m_pitch, m_yaw, 0.0f);
}

void PointCameraController::set_distance_to_point(float distance_to_point) {
    m_distance_to_point = distance_to_point;
    update_camera(1.0f);
}

void PointCameraController::update_controls(float dt, const sm::Ctx&) {
    static constexpr float MOVE_SPEED {2700.0f};
    static constexpr float MOVE_SPEED_MOUSE {MOVE_SPEED * 0.004f};
    static constexpr float ZOOM_SPEED {370.0f};
    static constexpr float ZOOM_SPEED_WHEEL {ZOOM_SPEED * 0.08f};

    m_velocity_zoom -= ZOOM_SPEED_WHEEL * m_input.mouse_wheel;

    if (m_input.key_r) {
        m_velocity_zoom -= ZOOM_SPEED * dt;
    } else if (m_input.key_f) {
        m_velocity_zoom += ZOOM_SPEED * dt;
    }

    if (m_input.mouse_right) {
        m_velocity_y -= MOVE_SPEED_MOUSE * m_input.dy;
        m_velocity_x += MOVE_SPEED_MOUSE * m_input.dx;
    }

    if (m_input.key_w) {
        m_velocity_y += MOVE_SPEED * dt;
    } else if (m_input.key_s) {
        m_velocity_y -= MOVE_SPEED * dt;
    }

    if (m_input.key_a) {
        m_velocity_x -= MOVE_SPEED * dt;
    } else if (m_input.key_d) {
        m_velocity_x += MOVE_SPEED * dt;
    }

    m_input.mouse_wheel = 0.0f;
    m_input.dx = 0.0f;
    m_input.dy = 0.0f;
}

void PointCameraController::update_camera(float dt) {
    m_distance_to_point += m_velocity_zoom * sensitivity * dt;

    // Calculate automatic distance to point movement
    calculate_auto_distance_to_point(dt);

    // Limit distance_to_point
    m_distance_to_point = glm::max(m_distance_to_point, MIN_DISTANCE_TO_POINT);
    m_distance_to_point = glm::min(m_distance_to_point, MAX_DISTANCE_TO_POINT);

    m_pitch += m_velocity_y * sensitivity * dt;

    // Calculate automatic pitch movement
    calculate_auto_pitch(dt);

    // Limit pitch
    m_pitch = glm::min(m_pitch, 90.0f);
    m_pitch = glm::max(m_pitch, -90.0f);

    m_angle_around_point += m_velocity_x * sensitivity * dt;

    // Calculate automatic angle around point movement
    calculate_auto_angle_around_point(dt);

    const float horizontal_distance {m_distance_to_point * glm::cos(glm::radians(m_pitch))};
    const float vertical_distance {m_distance_to_point * glm::sin(glm::radians(m_pitch))};

    const float offset_x {horizontal_distance * glm::sin(glm::radians(m_angle_around_point))};
    const float offset_z {horizontal_distance * glm::cos(glm::radians(m_angle_around_point))};

    m_position.x = m_point.x - offset_x;
    m_position.z = m_point.z - offset_z;
    m_position.y = m_point.y + vertical_distance;

    m_yaw = 180.0f - m_angle_around_point;

    // Update camera data
    glm::mat4 matrix {1.0f};
    matrix = glm::rotate(matrix, glm::radians(m_pitch), glm::vec3(1.0f, 0.0f, 0.0f));
    matrix = glm::rotate(matrix, glm::radians(m_yaw), glm::vec3(0.0f, 1.0f, 0.0f));
    matrix = glm::translate(matrix, -m_position);

    m_camera->set_view(matrix);
}

void PointCameraController::update_friction() {
    // Slow down velocity
    m_velocity_x *= FRICTION;
    m_velocity_y *= FRICTION;
    m_velocity_zoom *= FRICTION;
}

void PointCameraController::set_position(glm::vec3 position) {
    m_position = position;

    // Calculate distance to point
    m_distance_to_point = glm::length(position - m_point);

    // Update camera data
    m_camera->set_position_orientation(position, m_point - position, glm::vec3(0.0f, 1.0f, 0.0f));

    // Calculate yaw, pitch and angle_around_point
    const glm::vec3 direction {glm::vec3(glm::transpose(m_camera->view())[2])};
    m_yaw = glm::degrees(glm::atan(-direction.x, direction.z));
    m_pitch = glm::degrees(glm::asin(direction.y));
    m_angle_around_point = 180.0f - m_yaw;

    // Stop go_towards_position
    m_movement.auto_move_x = false;
    m_movement.auto_move_y = false;
    m_movement.auto_move_zoom = false;
}

void PointCameraController::go_towards_position(glm::vec3 position) {
    m_movement.cached_towards_position = position;
    m_movement.dont_auto_call_go_towards_position = false;

    // Calculate distance to point velocity
    go_towards_position_zoom(position);

    const glm::mat4 view {glm::lookAt(position, m_point - position, glm::vec3(0.0f, 1.0f, 0.0f))};
    const glm::vec3 direction {glm::vec3(glm::transpose(view)[2])};

    // Calculate pitch velocity
    go_towards_position_y(direction);

    // Calculate angle around point velocity
    go_towards_position_x(direction);
}

void PointCameraController::connect_events(sm::Ctx& ctx) {
    ctx.connect_event<sm::MouseWheelScrolledEvent, &PointCameraController::on_mouse_wheel_scrolled>(this);
    ctx.connect_event<sm::MouseMovedEvent, &PointCameraController::on_mouse_moved>(this);
    ctx.connect_event<sm::MouseButtonPressedEvent, &PointCameraController::on_mouse_button_pressed>(this);
    ctx.connect_event<sm::MouseButtonReleasedEvent, &PointCameraController::on_mouse_button_released>(this);
    ctx.connect_event<sm::KeyPressedEvent, &PointCameraController::on_key_pressed>(this);
    ctx.connect_event<sm::KeyReleasedEvent, &PointCameraController::on_key_released>(this);
    ctx.connect_event<sm::WindowMovedEvent, &PointCameraController::on_window_moved>(this);
}

void PointCameraController::disconnect_events(sm::Ctx& ctx) {
    ctx.disconnect_events(this);
}

void PointCameraController::on_mouse_wheel_scrolled(const sm::MouseWheelScrolledEvent& event) {
    m_input.mouse_wheel = event.scroll;
}

void PointCameraController::on_mouse_moved(const sm::MouseMovedEvent& event) {
    m_input.dx = m_input.last_mouse_x - event.mouse_x;
    m_input.dy = m_input.last_mouse_y - event.mouse_y;
    m_input.last_mouse_x = event.mouse_x;
    m_input.last_mouse_y = event.mouse_y;
}

void PointCameraController::on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event) {
    if (event.button == sm::MouseButton::Right) {
        m_input.mouse_right = true;
    }
}

void PointCameraController::on_mouse_button_released(const sm::MouseButtonReleasedEvent& event) {
    if (event.button == sm::MouseButton::Right) {
        m_input.mouse_right = false;
    }
}

void PointCameraController::on_key_pressed(const sm::KeyPressedEvent& event) {
    if (event.repeat) {
        return;
    }

    switch (event.key) {
        case sm::Key::W:
            m_input.key_w = true;
            break;
        case sm::Key::A:
            m_input.key_a = true;
            break;
        case sm::Key::S:
            m_input.key_s = true;
            break;
        case sm::Key::D:
            m_input.key_d = true;
            break;
        case sm::Key::R:
            m_input.key_r = true;
            break;
        case sm::Key::F:
            m_input.key_f = true;
            break;
        default:
            break;
    }
}

void PointCameraController::on_key_released(const sm::KeyReleasedEvent& event) {
    switch (event.key) {
        case sm::Key::W:
            m_input.key_w = false;
            break;
        case sm::Key::A:
            m_input.key_a = false;
            break;
        case sm::Key::S:
            m_input.key_s = false;
            break;
        case sm::Key::D:
            m_input.key_d = false;
            break;
        case sm::Key::R:
            m_input.key_r = false;
            break;
        case sm::Key::F:
            m_input.key_f = false;
            break;
        default:
            break;
    }
}

void PointCameraController::on_window_moved(const sm::WindowMovedEvent&) {
    m_input.key_w = false;
    m_input.key_a = false;
    m_input.key_s = false;
    m_input.key_d = false;
    m_input.key_r = false;
    m_input.key_f = false;
}

void PointCameraController::go_towards_position_x(glm::vec3 direction) {
    float integer_angle;
    const float fract {
        glm::modf(180.0f - glm::degrees(glm::atan(-direction.x, direction.z)), integer_angle)
    };

    m_movement.target_angle_around_point = static_cast<float>(static_cast<int>(integer_angle) % 360) - fract;

    if (m_movement.target_angle_around_point < 0.0f) {
        m_movement.target_angle_around_point += 360.0f;
    }

    const float angle {
        calculate_angle_velocity(
            m_movement.target_angle_around_point,
            m_angle_around_point
        )
    };

    m_movement.auto_x_velocity = angle * X_BASE_VELOCITY;
    m_movement.auto_move_x = true;
    m_movement.virtual_angle_around_point = m_angle_around_point;
    m_velocity_x = 0.0f;
}

void PointCameraController::go_towards_position_y(glm::vec3 direction) {
    m_movement.target_pitch = glm::degrees(glm::asin(direction.y));

    m_movement.auto_y_velocity = (m_movement.target_pitch - m_pitch) * Y_BASE_VELOCITY;
    m_movement.auto_move_y = true;
    m_movement.virtual_pitch = m_pitch;
    m_velocity_y = 0.0f;
}

void PointCameraController::go_towards_position_zoom(glm::vec3 position) {
    m_movement.target_distance_to_point = glm::length(position - m_point);

    m_movement.auto_zoom_velocity = (m_movement.target_distance_to_point - m_distance_to_point) * ZOOM_BASE_VELOCITY;
    m_movement.auto_move_zoom = true;
    m_movement.virtual_distance_to_point = m_distance_to_point;
    m_velocity_zoom = 0.0f;
}

void PointCameraController::calculate_auto_angle_around_point(float dt) {
    if (m_velocity_x < -0.1f || m_velocity_x > 0.1f) {
        m_movement.auto_move_x = false;
        m_movement.dont_auto_call_go_towards_position = true;
    }

    if (m_movement.auto_move_x) {
        const float angle {
            calculate_angle_velocity(
                m_movement.target_angle_around_point,
                m_movement.virtual_angle_around_point
            )
        };
        const float delta = m_movement.auto_x_velocity * dt + angle * X_VARIABLE_VELOCITY * dt;
        m_angle_around_point += delta;
        m_movement.virtual_angle_around_point += delta;
    }

    const float angle {
        calculate_angle_velocity(
            m_movement.target_angle_around_point,
            m_movement.virtual_angle_around_point
        )
    };

    if (m_movement.auto_move_x && glm::abs(angle) < 0.02f) {
        m_angle_around_point = m_movement.target_angle_around_point;
        m_movement.auto_move_x = false;
    } else if (m_movement.auto_move_x && glm::abs(angle) < 1.0f) {
        if (!m_movement.dont_auto_call_go_towards_position) {
            go_towards_position(m_movement.cached_towards_position);
        }
    }
}

void PointCameraController::calculate_auto_pitch(float dt) {
    if (m_velocity_y < -0.1f || m_velocity_y > 0.1f) {
        m_movement.auto_move_y = false;
        m_movement.dont_auto_call_go_towards_position = true;
    }

    if (m_movement.auto_move_y) {
        const float delta {
            m_movement.auto_y_velocity * dt + (m_movement.target_pitch - m_movement.virtual_pitch) * Y_VARIABLE_VELOCITY * dt
        };
        m_pitch += delta;
        m_movement.virtual_pitch += delta;
    }

    if (m_movement.auto_move_y && glm::abs(m_movement.target_pitch - m_movement.virtual_pitch) < 0.02f) {
        m_pitch = m_movement.target_pitch;
        m_movement.auto_move_y = false;
    } else if (m_movement.auto_move_y && glm::abs(m_movement.target_pitch - m_movement.virtual_pitch) < 1.0f) {
        if (!m_movement.dont_auto_call_go_towards_position) {
            go_towards_position(m_movement.cached_towards_position);
        }
    }
}

void PointCameraController::calculate_auto_distance_to_point(float dt) {
    if (m_velocity_zoom < -0.1f || m_velocity_zoom > 0.1f) {
        m_movement.auto_move_zoom = false;
        m_movement.dont_auto_call_go_towards_position = true;
    }

    if (m_movement.auto_move_zoom) {
        const float delta {
            m_movement.auto_zoom_velocity * dt + (m_movement.target_distance_to_point - m_movement.virtual_distance_to_point)
            * ZOOM_VARIABLE_VELOCITY * dt
        };
        m_distance_to_point += delta;
        m_movement.virtual_distance_to_point += delta;
    }

    if (m_movement.auto_move_zoom && glm::abs(m_movement.target_distance_to_point - m_movement.virtual_distance_to_point) < 0.02f) {
        m_distance_to_point = m_movement.target_distance_to_point;
        m_movement.auto_move_zoom = false;
    } else if (m_movement.auto_move_zoom && glm::abs(m_movement.target_distance_to_point - m_movement.virtual_distance_to_point) < 1.0f) {
        if (!m_movement.dont_auto_call_go_towards_position) {
            go_towards_position(m_movement.cached_towards_position);
        }
    }
}
