#pragma once

#include <glm/glm.hpp>

class Camera {  // TODO maybe refactor camera using quaternions
public:
    Camera() = default;
    ~Camera() = default;

    const glm::vec3& get_position() const { return position; }
    const glm::vec3& get_rotation() const { return rotation; }
    const glm::mat4& get_view_matrix() const { return view_matrix; }
    const glm::mat4& get_projection_matrix() const { return projection_matrix; }
    const glm::mat4& get_projection_view_matrix() const { return projection_view_matrix; }

    void set_projection(int width, int height, float fov, float near, float far);  // FIXME improve API
    void set_view(const glm::vec3& position, const glm::vec3& look, const glm::vec3& up, const glm::vec3& rotation);
    void set_view(const glm::mat4& view_matrix, const glm::vec3& position, const glm::vec3& rotation);

    void set_position(const glm::vec3& position);
    void set_rotation(const glm::vec3& rotation);
    void set_position_rotation(const glm::vec3& position, const glm::vec3& rotation);
private:
    void recalculate_view();

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);

    glm::mat4 view_matrix = glm::mat4(1.0f);
    glm::mat4 projection_matrix = glm::mat4(1.0f);
    glm::mat4 projection_view_matrix = glm::mat4(1.0f);  // This is a cache

    template<typename Archive>
    friend void serialize(Archive& archive, Camera& camera);
};
