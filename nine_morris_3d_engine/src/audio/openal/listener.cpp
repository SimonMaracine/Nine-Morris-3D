#include <AL/al.h>
#include <glm/glm.hpp>

#include "engine/audio/openal/listener.hpp"
#include "engine/audio/openal/info_and_debug.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    void AlListener::set_gain(float gain) {
        SM_ASSERT(gain >= 0.0f, "Must be positive");

        alListenerf(AL_GAIN, gain);

        AlInfoDebug::maybe_check_errors();

        this->gain = gain;
    }

    void AlListener::set_position(const glm::vec3& position) {
        alListener3f(AL_POSITION, position.x, position.y, position.z);

        AlInfoDebug::maybe_check_errors();

        this->position = position;
    }

    void AlListener::set_velocity(const glm::vec3& velocity) {
        alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);

        AlInfoDebug::maybe_check_errors();

        this->velocity = velocity;
    }

    void AlListener::set_look_at_and_up(const glm::vec3& look_at, const glm::vec3& up) {
        const float orientation[6] {
            look_at.x,
            look_at.y,
            look_at.z,
            up.x,
            up.y,
            up.z
        };

        alListenerfv(AL_ORIENTATION, orientation);

        AlInfoDebug::maybe_check_errors();

        this->look_at = look_at;
        this->up = up;
    }

    void AlListener::set_distance_model(DistanceModel distance_model) {
        ALenum result {0};

        switch (distance_model) {
            case DistanceModel::None:
                result = AL_NONE;
                break;
            case DistanceModel::Exponent:
                result = AL_EXPONENT_DISTANCE;
                break;
            case DistanceModel::Inverse:
                result = AL_INVERSE_DISTANCE;
                break;
            case DistanceModel::Linear:
                result = AL_LINEAR_DISTANCE;
                break;
            case DistanceModel::ExponentClamped:
                result = AL_EXPONENT_DISTANCE_CLAMPED;
                break;
            case DistanceModel::InverseClamped:
                result = AL_INVERSE_DISTANCE_CLAMPED;
                break;
            case DistanceModel::LinearClamped:
                result = AL_LINEAR_DISTANCE_CLAMPED;
                break;
        }

        alDistanceModel(result);

        AlInfoDebug::maybe_check_errors();

        this->distance_model = distance_model;
    }
}
