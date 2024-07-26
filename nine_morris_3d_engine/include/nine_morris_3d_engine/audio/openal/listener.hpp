#pragma once

#include <glm/glm.hpp>

namespace sm {
    namespace listener {
        enum class DistanceModel {
            None,
            Exponent,
            Inverse,
            Linear,
            ExponentClamped,
            InverseClamped,
            LinearClamped
        };

        void set_gain(float gain) noexcept;
        void set_position(glm::vec3 position) noexcept;
        void set_velocity(glm::vec3 velocity) noexcept;
        void set_look_at_and_up(glm::vec3 look_at, glm::vec3 up) noexcept;
        void set_distance_model(DistanceModel distance_model) noexcept;

        float get_gain() noexcept;
        glm::vec3 get_position() noexcept;
        glm::vec3 get_velocity() noexcept;
        glm::vec3 get_look_at() noexcept;
        glm::vec3 get_up() noexcept;
        DistanceModel get_distance_model() noexcept;
    }
}
