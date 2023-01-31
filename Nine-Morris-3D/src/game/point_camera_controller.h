#pragma once

#include <engine/engine_application.h>
#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

inline constexpr float LENS_FOV = 45.0f;
inline constexpr float LENS_NEAR = 0.1f;
inline constexpr float LENS_FAR = 90.0f;

class PointCameraController : public CameraController {
public:
    PointCameraController() = default;
    PointCameraController(Camera* camera);
    PointCameraController(Camera* camera, int width, int height, float fov, float near, float far,
        const glm::vec3& point, float distance_to_point, float pitch, float sensitivity);
    virtual ~PointCameraController() = default;

    virtual const glm::vec3& get_position() const override;
    virtual const glm::vec3& get_rotation() const override;

    const glm::vec3& get_point() const { return point; }
    float get_distance_to_point() const { return distance_to_point; }
    float get_angle_around_point() const { return angle_around_point; }

    // Call these every frame
    virtual void update_controls(float dt) override;
    virtual void update_camera(float dt) override;

    // Call this regularly
    void update_friction();

    // Directly set this position and linearly interpolate to this position respectively
    void set_position(const glm::vec3& position);
    void go_towards_position(const glm::vec3& position);

    void setup_events(Application* app);
    void remove_events(Application* app);
    void discard_events(Application* app);

    float sensitivity = 1.0f;  // Best from 0.5 to 2.0
private:
    void on_mouse_scrolled(const MouseScrolledEvent& event);
    void on_mouse_moved(const MouseMovedEvent& event);

    void go_towards_position_x(const glm::vec3& direction);
    void go_towards_position_y(const glm::vec3& direction);
    void go_towards_position_zoom(const glm::vec3& position);

    void calculate_auto_angle_around_point(float dt);
    void calculate_auto_pitch(float dt);
    void calculate_auto_distance_to_point(float dt);

    glm::vec3 position = glm::vec3(0.0f);
    float pitch = 0.0f;
    float yaw = 0.0f;

    glm::vec3 point = glm::vec3(0.0f);
    float distance_to_point = 0.0f;
    float angle_around_point = 0.0f;

    float x_velocity = 0.0f;
    float y_velocity = 0.0f;
    float zoom_velocity = 0.0f;

    // Interpolated movement stuff
    struct {
        bool auto_move_x = false;
        float target_angle_around_point = 0.0f;
        float auto_x_velocity = 0.0f;
        float virtual_angle_around_point = 0.0f;

        bool auto_move_y = false;
        float target_pitch = 0.0f;
        float auto_y_velocity = 0.0f;
        float virtual_pitch = 0.0f;

        bool auto_move_zoom = false;
        float target_distance_to_point = 0.0f;
        float auto_zoom_velocity = 0.0f;
        float virtual_distance_to_point = 0.0f;

        glm::vec3 cached_towards_position = glm::vec3(0.0f);
        bool dont_auto_call_go_towards_position = false;
    } movement;

    struct {
        float mouse_wheel = 0.0f;
        float dx = 0.0f;
        float dy = 0.0f;
        float last_mouse_x = 0.0f;
        float last_mouse_y = 0.0f;
    } mouse_input;

    template<typename Archive>
    friend void serialize(Archive& archive, PointCameraController& camera_controller);
};
