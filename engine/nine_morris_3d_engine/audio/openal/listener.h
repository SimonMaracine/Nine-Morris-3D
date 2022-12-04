#pragma once

#include <glm/glm.hpp>

namespace al {
    class Listener {
    public:
        Listener() = default;  // TODO must set defaults?
        ~Listener() = default;

        Listener(const Listener&) = delete;
        Listener& operator=(const Listener&) = delete;
        Listener(Listener&&) = delete;
        Listener& operator=(Listener&&) = delete;

        float get_gain() const { return gain; }
        const glm::vec3& get_position() const { return position; }
        const glm::vec3& get_velocity() const { return velocity; }
        const glm::vec3& get_look_at() const { return look_at; }
        const glm::vec3& get_up() const { return up; }

        void set_gain(float gain);
        void set_position(const glm::vec3& position);
        void set_velocity(const glm::vec3& velocity);
        void set_look_at(const glm::vec3& look_at);
        void set_up(const glm::vec3& up);
    private:
        float gain = 1.0f;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 look_at = glm::vec3(0.0f);  // FIXME default?
        glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
    };
}
