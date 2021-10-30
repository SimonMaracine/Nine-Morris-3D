#include <glm/glm.hpp>

#include "opengl/renderer/camera.h"

Camera::Camera(const glm::mat4& projection_matrix, const glm::vec3& point, float distance_to_point)
    : projection_matrix(projection_matrix), point(point), distance_to_point(distance_to_point) {

}
