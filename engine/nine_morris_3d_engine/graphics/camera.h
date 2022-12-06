#pragma once

#include <glm/glm.hpp>

class Camera {  // TODO maybe refactor camera using quaternions
public:
    Camera() = default;
    ~Camera() = default;

    const glm::mat4& get_view_matrix() const { return view_matrix; }
    const glm::mat4& get_projection_matrix() const { return projection_matrix; }
    const glm::mat4& get_projection_view_matrix() const { return projection_view_matrix; }

    void set_projection(int width, int height, float fov, float near, float far);
    void set_view(const glm::vec3& position, const glm::vec3& at, const glm::vec3& up);
    void set_view(const glm::mat4& view_matrix);

    void set_position_rotation(const glm::vec3& position, const glm::vec3& rotation);
private:
    void recalculate_view(const glm::vec3& position, const glm::vec3& rotation);

    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 projection_matrix = glm::mat4(1.0f);
    glm::mat4 projection_view_matrix = glm::mat4(1.0f);  // This is a cache
};
