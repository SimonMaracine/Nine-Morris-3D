#pragma once

#include <glm/glm.hpp>

class Camera {  // TODO maybe refactor camera using quaternions
public:
    Camera() = default;
    Camera(float sensitivity, float pitch, const glm::vec3& point, float distance_to_point,
        const glm::mat4& projection_matrix);
    ~Camera() = default;

    // Call these every frame
    void update_controls(float mouse_wheel, float dx, float dy, float dt);
    void update(float dt);

    // Call this regularly
    void update_friction();

    // Call this whenever needed (usually every frame)
    void update_projection(float width, float height);

    // Directly set this position and linearly interpolate to this position respectively
    void set_position(const glm::vec3& position);
    void go_towards_position(const glm::vec3& position);

    const glm::vec3& get_position() const { return position; }
    float get_pitch() const { return pitch; }
    float get_yaw() const { return yaw; }
    const glm::vec3& get_point() const { return point; }
    float get_distance_to_point() const { return distance_to_point; }
    float get_angle_around_point() const { return angle_around_point; }
    const glm::mat4& get_view_matrix() const { return view_matrix; }
    const glm::mat4& get_projection_matrix() const { return projection_matrix; }
    const glm::mat4& get_projection_view_matrix() const { return projection_view_matrix; }

    float sensitivity = 1.0f;  // From 0.5 to 2.0

    static constexpr float LENS_FOV = 45.0f;
    static constexpr float LENS_NEAR = 0.1f;
    static constexpr float LENS_FAR = 90.0f;
private:
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

    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 projection_matrix = glm::mat4(1.0f);
    glm::mat4 projection_view_matrix = glm::mat4(1.0f);  // This is a cache

    float x_velocity = 0.0f;
    float y_velocity = 0.0f;
    float zoom_velocity = 0.0f;

    // Interpolated movement stuff
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

    template<typename Archive>
    friend void serialize(Archive& archive, Camera& camera);
};
