#pragma once

#include <AL/al.h>
#include <glm/glm.hpp>

#include "nine_morris_3d_engine/audio/openal/buffer.h"

namespace al {
    class Source {
    public:
        Source();
        ~Source();

        Source(const Source&) = delete;
        Source& operator=(const Source&) = delete;
        Source(Source&&) = delete;
        Source& operator=(Source&&) = delete;

        void play(Buffer* buffer);
        void stop();
        void pause_playing();
        void continue_playing();

        float get_gain() { return gain; }
        float get_pitch() { return pitch; }
        float get_rolloff_factor() { return rolloff_factor; }
        const glm::vec3& get_position() { return position; }
        const glm::vec3& get_velocity() { return velocity; }
        const glm::vec3& get_direction() { return direction; }
        bool get_looping() { return looping; }

        bool is_playing();

        void set_gain(float gain);
        void set_pitch(float pitch);
        void set_rolloff_factor(float rolloff_factor);
        void set_position(const glm::vec3& position);
        void set_velocity(const glm::vec3& velocity);
        void set_direction(const glm::vec3& direction);
        void set_looping(bool looping);
    private:
        ALuint source = 0;
        ALuint attached_buffer = 0;

        float gain = 1.0f;
        float pitch = 1.0f;
        float rolloff_factor = 1.0f;
        glm::vec3 position = glm::vec3(0.0f);
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 direction = glm::vec3(0.0f);
        bool looping = false;
    };
}
