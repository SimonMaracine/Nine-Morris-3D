#pragma once

#include <glm/glm.hpp>

namespace sm {
    enum class DistanceModel {
        None,
        Exponent,
        Inverse,
        Linear,
        ExponentClamped,
        InverseClamped,
        LinearClamped
    };

    class AlListener {
    public:
        float get_gain() const { return gain; }
        glm::vec3 get_position() const { return position; }
        glm::vec3 get_velocity() const { return velocity; }
        glm::vec3 get_look_at() const { return look_at; }
        glm::vec3 get_up() const { return up; }
        DistanceModel get_distance_model() const { return distance_model; }

        void set_gain(float gain);
        void set_position(glm::vec3 position);
        void set_velocity(glm::vec3 velocity);
        void set_look_at_and_up(glm::vec3 look_at, glm::vec3 up);
        void set_distance_model(DistanceModel distance_model);
    private:
        float gain {1.0f};
        glm::vec3 position {};
        glm::vec3 velocity {};
        glm::vec3 look_at {0.0f, 0.0f, -1.0f};
        glm::vec3 up {0.0f, 1.0f, 0.0f};
        DistanceModel distance_model {DistanceModel::InverseClamped};
    };
}
