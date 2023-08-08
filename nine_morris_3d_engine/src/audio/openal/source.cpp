#include <AL/al.h>

#include "engine/audio/openal/source.h"
#include "engine/audio/openal/info_and_debug.h"
#include "engine/other/logging.h"
#include "engine/other/assert.h"

namespace sm {
    namespace al {
        AlSource::AlSource() {
            alGenSources(1, &source);
            alSourcef(source, AL_ROLLOFF_FACTOR, rolloff_factor);
            alSourcef(source, AL_REFERENCE_DISTANCE, reference_distance);
            alSourcef(source, AL_MAX_DISTANCE, max_distance);

            maybe_check_errors();

            LOG_DEBUG("Created AL source {}", source);
        }

        AlSource::~AlSource() {
            stop();

            alDeleteSources(1, &source);

            maybe_check_errors();

            LOG_DEBUG("Deleted AL source {}", source);
        }

        void AlSource::play(Buffer* buffer) {
            stop();

            if (buffer->buffer != attached_buffer) {
                attached_buffer = buffer->buffer;
                alSourcei(source, AL_BUFFER, attached_buffer);

                maybe_check_errors();

                buffer->sources_attached.insert(source);
            }

            alSourcePlay(source);

            maybe_check_errors();
        }

        void AlSource::stop() {
            alSourceStop(source);

            maybe_check_errors();
        }

        void AlSource::pause() {
            alSourcePause(source);

            maybe_check_errors();
        }

        void AlSource::continue_() {
            alSourcePlay(source);

            maybe_check_errors();
        }

        bool AlSource::is_playing() {
            int state = 0;
            alGetSourcei(source, AL_SOURCE_STATE, &state);

            maybe_check_errors();

            return state == AL_PLAYING;
        }

        void AlSource::set_gain(float gain) {
            ASSERT(gain >= 0.0f, "Must be positive");

            alSourcef(source, AL_GAIN, gain);

            maybe_check_errors();

            this->gain = gain;
        }

        void AlSource::set_pitch(float pitch) {
            ASSERT(pitch >= 0.0f, "Must be positive");

            alSourcef(source, AL_PITCH, pitch);

            maybe_check_errors();

            this->pitch = pitch;
        }

        void AlSource::set_position(const glm::vec3& position) {
            alSource3f(source, AL_POSITION, position.x, position.y, position.z);

            maybe_check_errors();

            this->position = position;
        }

        void AlSource::set_velocity(const glm::vec3& velocity) {
            alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);

            maybe_check_errors();

            this->velocity = velocity;
        }

        void AlSource::set_direction(const glm::vec3& direction) {
            alSource3f(source, AL_DIRECTION, direction.x, direction.y, direction.z);

            maybe_check_errors();

            this->direction = direction;
        }

        void AlSource::set_looping(bool looping) {
            alSourcei(source, AL_LOOPING, static_cast<ALint>(looping));

            maybe_check_errors();

            this->looping = looping;
        }

        void AlSource::set_rolloff_factor(float rolloff_factor) {
            alSourcef(source, AL_ROLLOFF_FACTOR, rolloff_factor);

            maybe_check_errors();

            this->rolloff_factor = rolloff_factor;
        }

        void AlSource::set_reference_distance(float reference_distance) {
            alSourcef(source, AL_REFERENCE_DISTANCE, reference_distance);

            maybe_check_errors();

            this->reference_distance = reference_distance;
        }

        void AlSource::set_max_distance(float max_distance) {
            alSourcef(source, AL_MAX_DISTANCE, max_distance);

            maybe_check_errors();

            this->max_distance = max_distance;
        }
    }
}
