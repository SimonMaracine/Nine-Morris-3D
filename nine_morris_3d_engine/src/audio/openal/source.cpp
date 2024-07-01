#include "nine_morris_3d_engine/audio/openal/source.hpp"

#include <cassert>

#include <AL/al.h>

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/debug.hpp"

namespace sm {
    AlSource::AlSource() {
        alGenSources(1, &source);
        alSourcef(source, AL_ROLLOFF_FACTOR, rolloff_factor);
        alSourcef(source, AL_REFERENCE_DISTANCE, reference_distance);
        alSourcef(source, AL_MAX_DISTANCE, max_distance);

        openal_debug::check_errors();

        LOG_DEBUG("Created AL source {}", source);
    }

    AlSource::~AlSource() {
        stop();

        alDeleteSources(1, &source);

        openal_debug::check_errors();

        LOG_DEBUG("Deleted AL source {}", source);
    }

    void AlSource::play(AlBuffer* buffer) {
        stop();

        if (buffer->buffer != attached_buffer) {
            attached_buffer = buffer->buffer;
            alSourcei(source, AL_BUFFER, attached_buffer);

            openal_debug::check_errors();

            buffer->sources_attached.insert(source);
        }

        alSourcePlay(source);

        openal_debug::check_errors();
    }

    void AlSource::stop() const {
        alSourceStop(source);

        openal_debug::check_errors();
    }

    void AlSource::pause() const {
        alSourcePause(source);

        openal_debug::check_errors();
    }

    void AlSource::resume() const {
        alSourcePlay(source);

        openal_debug::check_errors();
    }

    bool AlSource::is_playing() const {
        int state {};
        alGetSourcei(source, AL_SOURCE_STATE, &state);

        openal_debug::check_errors();

        return state == AL_PLAYING;
    }

    void AlSource::set_gain(float gain) {
        assert(gain >= 0.0f);

        alSourcef(source, AL_GAIN, gain);

        openal_debug::check_errors();

        this->gain = gain;
    }

    void AlSource::set_pitch(float pitch) {
        assert(pitch >= 0.0f);

        alSourcef(source, AL_PITCH, pitch);

        openal_debug::check_errors();

        this->pitch = pitch;
    }

    void AlSource::set_position(glm::vec3 position) {
        alSource3f(source, AL_POSITION, position.x, position.y, position.z);

        openal_debug::check_errors();

        this->position = position;
    }

    void AlSource::set_velocity(glm::vec3 velocity) {
        alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);

        openal_debug::check_errors();

        this->velocity = velocity;
    }

    void AlSource::set_direction(glm::vec3 direction) {
        alSource3f(source, AL_DIRECTION, direction.x, direction.y, direction.z);

        openal_debug::check_errors();

        this->direction = direction;
    }

    void AlSource::set_looping(bool looping) {
        alSourcei(source, AL_LOOPING, static_cast<ALint>(looping));

        openal_debug::check_errors();

        this->looping = looping;
    }

    void AlSource::set_rolloff_factor(float rolloff_factor) {
        alSourcef(source, AL_ROLLOFF_FACTOR, rolloff_factor);

        openal_debug::check_errors();

        this->rolloff_factor = rolloff_factor;
    }

    void AlSource::set_reference_distance(float reference_distance) {
        alSourcef(source, AL_REFERENCE_DISTANCE, reference_distance);

        openal_debug::check_errors();

        this->reference_distance = reference_distance;
    }

    void AlSource::set_max_distance(float max_distance) {
        alSourcef(source, AL_MAX_DISTANCE, max_distance);

        openal_debug::check_errors();

        this->max_distance = max_distance;
    }
}
