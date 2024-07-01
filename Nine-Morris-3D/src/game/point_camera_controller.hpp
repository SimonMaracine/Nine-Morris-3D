#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <nine_morris_3d_engine/external/glm.h++>

inline constexpr float LENS_FOV {45.0f};
inline constexpr float LENS_NEAR {1.0f};
inline constexpr float LENS_FAR {200.0f};

// TODO maybe refactor camera using quaternions
class PointCameraController : public sm::CameraController {
public:
    PointCameraController() = default;
    explicit PointCameraController(sm::Camera* camera);
    PointCameraController(
        sm::Camera* camera,
        int width,
        int height,
        float fov,
        float near,
        float far,
        glm::vec3 point,
        float distance_to_point,
        float pitch,
        float sensitivity
    );

    glm::vec3 get_position() const override;
    glm::vec3 get_rotation() const override;

    glm::vec3 get_point() const { return point; }
    float get_distance_to_point() const { return distance_to_point; }
    float get_angle_around_point() const { return angle_around_point; }

    // Call these every frame
    void update_controls(float dt, const sm::Input& inp) override;
    void update_camera(float dt) override;

    // Call this regularly
    void update_friction();

    // Directly set this position and linearly interpolate to this position respectively
    void set_position(glm::vec3 position);
    void go_towards_position(glm::vec3 position);

    void connect_events(sm::Ctx& ctx);
    void disconnect_events(sm::Ctx& ctx);
    void discard_events(sm::Ctx& ctx);

    float sensitivity {1.0f};  // Best from 0.5 to 2.0
private:
    void on_mouse_wheel_scrolled(const sm::MouseWheelScrolledEvent& event);
    void on_mouse_moved(const sm::MouseMovedEvent& event);

    void go_towards_position_x(glm::vec3 direction);
    void go_towards_position_y(glm::vec3 direction);
    void go_towards_position_zoom(glm::vec3 position);

    void calculate_auto_angle_around_point(float dt);
    void calculate_auto_pitch(float dt);
    void calculate_auto_distance_to_point(float dt);

    glm::vec3 position {};
    float pitch {};
    float yaw {};

    glm::vec3 point {};
    float distance_to_point {};
    float angle_around_point {};

    float x_velocity {};
    float y_velocity {};
    float zoom_velocity {};

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
    } movement;

    struct {
        float mouse_wheel {};
        float dx {};
        float dy {};
        float last_mouse_x {};
        float last_mouse_y {};
    } mouse_input;
};
