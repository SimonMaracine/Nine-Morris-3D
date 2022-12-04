#include <AL/al.h>
#include <glm/glm.hpp>

#include "nine_morris_3d_engine/audio/openal/listener.h"
#include "nine_morris_3d_engine/audio/openal/info_and_debug.h"
#include "nine_morris_3d_engine/other/assert.h"

namespace al {
    void Listener::set_gain(float gain) {
        ASSERT(gain >= 0.0f, "Must be positive");

        alListenerf(AL_GAIN, gain);

        maybe_check_errors();

        this->gain = gain;
    }

    void Listener::set_position(const glm::vec3& position) {
        alListener3f(AL_POSITION, position.x, position.y, position.z);

        maybe_check_errors();

        this->position = position;
    }

    void Listener::set_velocity(const glm::vec3& velocity) {
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);

        maybe_check_errors();

        this->velocity = velocity;
    }

    void Listener::set_look_at(const glm::vec3& look_at) {
        const float orientation[6] = {
            look_at.x, look_at.y, look_at.z,
            up.x, up.y, up.z
        };

        alListenerfv(AL_ORIENTATION, orientation);

        maybe_check_errors();

        this->look_at = look_at;
    }

    void Listener::set_up(const glm::vec3& up) {
        const float orientation[6] = {
            look_at.x, look_at.y, look_at.z,
            up.x, up.y, up.z
        };

        alListenerfv(AL_ORIENTATION, orientation);

        maybe_check_errors();

        this->up = up;
    }
}
