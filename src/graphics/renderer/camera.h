#pragma once

#include <glm/glm.hpp>

constexpr float FOV = 45.0f;
constexpr float NEAR = 0.1f;
constexpr float FAR = 70.0f;

class Camera {
public:
    Camera() = default;
    Camera(float pitch, const glm::vec3& point, float distance_to_point, const glm::mat4& projection_matrix,
            float sensitivity);
    ~Camera() = default;

    void update(float mouse_wheel, float dx, float dy, float dt);
    void update_friction();
    void update_projection(float width, float height);

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

    float sensitivity = 1.0f;  // From 0.5 to 2.0
};
