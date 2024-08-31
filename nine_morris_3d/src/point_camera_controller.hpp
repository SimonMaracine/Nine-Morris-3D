#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <nine_morris_3d_engine/external/glm.h++>

inline constexpr float LENS_FOV {45.0f};
inline constexpr float LENS_NEAR {1.0f};
inline constexpr float LENS_FAR {100.0f};

// TODO maybe refactor camera using quaternions
class PointCameraController : public sm::CameraController {
public:
    PointCameraController() = default;
    explicit PointCameraController(sm::Camera& camera);
    PointCameraController(
        sm::Camera& camera,
        int width,
        int height,
        glm::vec3 point,
        float distance_to_point,
        float pitch,
        float sensitivity
    );

    glm::vec3 get_position() const noexcept override;
    glm::vec3 get_rotation() const noexcept override;

    glm::vec3 get_point() const { return m_point; }
    float get_distance_to_point() const { return m_distance_to_point; }
    float get_angle_around_point() const { return m_angle_around_point; }

    void set_distance_to_point(float distance_to_point);

    // Call these every frame
    void update_controls(float dt, const sm::Ctx&) override;
    void update_camera(float dt) override;

    // Call this regularly
    void update_friction();

    // Directly set this position and linearly interpolate to this position respectively
    void set_position(glm::vec3 position);
    void go_towards_position(glm::vec3 position);

    void connect_events(sm::Ctx& ctx);
    void disconnect_events(sm::Ctx& ctx);

    float sensitivity {1.0f};  // Best from 0.5 to 2.0
private:
    void on_mouse_wheel_scrolled(const sm::MouseWheelScrolledEvent& event);
    void on_mouse_moved(const sm::MouseMovedEvent& event);
    void on_mouse_button_pressed(const sm::MouseButtonPressedEvent& event);
    void on_mouse_button_released(const sm::MouseButtonReleasedEvent& event);
    void on_key_pressed(const sm::KeyPressedEvent& event);
    void on_key_released(const sm::KeyReleasedEvent& event);
    void on_window_moved(const sm::WindowMovedEvent&);

    void go_towards_position_x(glm::vec3 direction);
    void go_towards_position_y(glm::vec3 direction);
    void go_towards_position_zoom(glm::vec3 position);

    void calculate_auto_angle_around_point(float dt);
    void calculate_auto_pitch(float dt);
    void calculate_auto_distance_to_point(float dt);

    glm::vec3 m_position {};
    float m_pitch {};
    float m_yaw {};

    glm::vec3 m_point {};
    float m_distance_to_point {};
    float m_angle_around_point {};

    float m_velocity_x {};
    float m_velocity_y {};
    float m_velocity_zoom {};

    // Interpolated movement stuff
    struct {
        bool auto_move_x {false};
        float target_angle_around_point {};
        float auto_x_velocity {};
        float virtual_angle_around_point {};

        bool auto_move_y {false};
        float target_pitch {};
        float auto_y_velocity {};
        float virtual_pitch {};

        bool auto_move_zoom {false};
        float target_distance_to_point {};
        float auto_zoom_velocity {};
        float virtual_distance_to_point {};

        glm::vec3 cached_towards_position {};
        bool dont_auto_call_go_towards_position {false};
    } m_movement;

    struct {
        float mouse_wheel {};
        float dx {};
        float dy {};
        float last_mouse_x {};
        float last_mouse_y {};
        bool mouse_right {false};
        bool key_w {false};
        bool key_a {false};
        bool key_s {false};
        bool key_d {false};
        bool key_r {false};
        bool key_f {false};
    } m_input;
};
