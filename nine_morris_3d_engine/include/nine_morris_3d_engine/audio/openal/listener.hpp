#pragma once

#include <glm/glm.hpp>

namespace sm::listener {
    enum class DistanceModel {
        None,
        Exponent,
        Inverse,
        Linear,
        ExponentClamped,
        InverseClamped,
        LinearClamped
    };

    void set_gain(float gain);
    void set_position(glm::vec3 position);
    void set_velocity(glm::vec3 velocity);
    void set_look_at_and_up(glm::vec3 look_at, glm::vec3 up);
    void set_distance_model(DistanceModel distance_model);

    float get_gain();
    glm::vec3 get_position();
    glm::vec3 get_velocity();
    glm::vec3 get_look_at();
    glm::vec3 get_up();
    DistanceModel get_distance_model();
}
