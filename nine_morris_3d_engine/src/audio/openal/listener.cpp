#include <AL/al.h>
#include <glm/glm.hpp>

#include "engine/audio/openal/listener.hpp"
#include "engine/audio/openal/info_and_debug.hpp"

namespace sm {
    namespace al {
        void AlListener::set_gain(float gain) {
            ASSERT(gain >= 0.0f, "Must be positive");

            alListenerf(AL_GAIN, gain);

            maybe_check_errors();

            this->gain = gain;
        }

        void AlListener::set_position(const glm::vec3& position) {
            alListener3f(AL_POSITION, position.x, position.y, position.z);

            maybe_check_errors();

            this->position = position;
        }

        void AlListener::set_velocity(const glm::vec3& velocity) {
            alListener3f(AL_VELOCITY, velocity.x, velocity.y, velocity.z);

            maybe_check_errors();

            this->velocity = velocity;
        }

        void AlListener::set_look_at_and_up(const glm::vec3& look_at, const glm::vec3& up) {
            const float orientation[6] = {
                look_at.x, look_at.y, look_at.z,
                up.x, up.y, up.z
            };

            alListenerfv(AL_ORIENTATION, orientation);

            maybe_check_errors();

            this->look_at = look_at;
            this->up = up;
        }

        void AlListener::set_distance_model(DistanceModel distance_model) {
            ALenum model = 0;

            switch (distance_model) {
                case DistanceModel::None:
                    model = AL_NONE;
                    break;
                case DistanceModel::Exponent:
                    model = AL_EXPONENT_DISTANCE;
                    break;
                case DistanceModel::Inverse:
                    model = AL_INVERSE_DISTANCE;
                    break;
                case DistanceModel::Linear:
                    model = AL_LINEAR_DISTANCE;
                    break;
                case DistanceModel::ExponentClamped:
                    model = AL_EXPONENT_DISTANCE_CLAMPED;
                    break;
                case DistanceModel::InverseClamped:
                    model = AL_INVERSE_DISTANCE_CLAMPED;
                    break;
                case DistanceModel::LinearClamped:
                    model = AL_LINEAR_DISTANCE_CLAMPED;
                    break;
            }

            alDistanceModel(model);

            maybe_check_errors();

            this->distance_model = distance_model;
        }
    }
}
