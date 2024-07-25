#include "nine_morris_3d_engine/audio/openal/source.hpp"

#include <cassert>

#include <AL/al.h>

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/debug.hpp"

namespace sm {
    AlSource::AlSource() {
        alGenSources(1, &source);
        alSourcef(source, AL_ROLLOFF_FACTOR, 1.0f);
        alSourcef(source, AL_REFERENCE_DISTANCE, 8.0f);
        alSourcef(source, AL_MAX_DISTANCE, 22.0f);

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
            buffer->sources_attached.insert(source);

            alSourcei(source, AL_BUFFER, attached_buffer);

            openal_debug::check_errors();
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

    void AlSource::set_gain(float gain) const {
        assert(gain >= 0.0f);

        alSourcef(source, AL_GAIN, gain);

        openal_debug::check_errors();
    }

    void AlSource::set_pitch(float pitch) const {
        assert(pitch >= 0.0f);

        alSourcef(source, AL_PITCH, pitch);

        openal_debug::check_errors();
    }

    void AlSource::set_position(glm::vec3 position) const {
        alSource3f(source, AL_POSITION, position.x, position.y, position.z);

        openal_debug::check_errors();
    }

    void AlSource::set_velocity(glm::vec3 velocity) const {
        alSource3f(source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);

        openal_debug::check_errors();
    }

    void AlSource::set_direction(glm::vec3 direction) const {
        alSource3f(source, AL_DIRECTION, direction.x, direction.y, direction.z);

        openal_debug::check_errors();
    }

    void AlSource::set_looping(bool looping) const {
        alSourcei(source, AL_LOOPING, static_cast<ALint>(looping));

        openal_debug::check_errors();
    }

    void AlSource::set_rolloff_factor(float rolloff_factor) const {
        alSourcef(source, AL_ROLLOFF_FACTOR, rolloff_factor);

        openal_debug::check_errors();
    }

    void AlSource::set_reference_distance(float reference_distance) const {
        alSourcef(source, AL_REFERENCE_DISTANCE, reference_distance);

        openal_debug::check_errors();
    }

    void AlSource::set_max_distance(float max_distance) const {
        alSourcef(source, AL_MAX_DISTANCE, max_distance);

        openal_debug::check_errors();
    }

    float AlSource::get_gain() const {
        float gain {};
        alGetSourcef(source, AL_GAIN, &gain);

        openal_debug::check_errors();

        return gain;
    }

    float AlSource::get_pitch() const {
        float pitch {};
        alGetSourcef(source, AL_PITCH, &pitch);

        openal_debug::check_errors();

        return pitch;
    }

    glm::vec3 AlSource::get_position() const {
        glm::vec3 position {};
        alGetSource3f(source, AL_POSITION, &position.x, &position.y, &position.z);

        openal_debug::check_errors();

        return position;
    }

    glm::vec3 AlSource::get_velocity() const {
        glm::vec3 velocity {};
        alGetSource3f(source, AL_VELOCITY, &velocity.x, &velocity.y, &velocity.z);

        openal_debug::check_errors();

        return velocity;
    }

    glm::vec3 AlSource::get_direction() const {
        glm::vec3 direction {};
        alGetSource3f(source, AL_DIRECTION, &direction.x, &direction.y, &direction.z);

        openal_debug::check_errors();

        return direction;
    }

    bool AlSource::get_looping() const {
        int looping {};
        alGetSourcei(source, AL_LOOPING, &looping);

        openal_debug::check_errors();

        return static_cast<bool>(looping);
    }

    float AlSource::get_rolloff_factor() const {
        float rolloff_factor {};
        alGetSourcef(source, AL_ROLLOFF_FACTOR, &rolloff_factor);

        openal_debug::check_errors();

        return rolloff_factor;
    }

    float AlSource::get_reference_distance() const {
        float reference_distance {};
        alGetSourcef(source, AL_REFERENCE_DISTANCE, &reference_distance);

        openal_debug::check_errors();

        return reference_distance;
    }

    float AlSource::get_max_distance() const {
        float max_distance {};
        alGetSourcef(source, AL_MAX_DISTANCE, &max_distance);

        openal_debug::check_errors();

        return max_distance;
    }
}
