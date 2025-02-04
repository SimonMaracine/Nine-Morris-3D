#include "nine_morris_3d_engine/audio/openal/source.hpp"

#include <cassert>

#include <AL/al.h>

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/debug.hpp"

namespace sm {
    AlSource::AlSource() {
        alGenSources(1, &m_source);
        alSourcef(m_source, AL_ROLLOFF_FACTOR, 1.0f);
        alSourcef(m_source, AL_REFERENCE_DISTANCE, 8.0f);
        alSourcef(m_source, AL_MAX_DISTANCE, 22.0f);

        openal_debug::check_errors();

        LOG_DEBUG("Created AL source {}", m_source);
    }

    AlSource::~AlSource() {
        stop();

        alDeleteSources(1, &m_source);

        openal_debug::check_errors();

        LOG_DEBUG("Deleted AL source {}", m_source);
    }

    void AlSource::play(std::shared_ptr<AlBuffer> buffer) {
        play(buffer.get());
    }

    void AlSource::play(const std::unique_ptr<AlBuffer>& buffer) {
        play(buffer.get());
    }

    void AlSource::stop() const {
        alSourceStop(m_source);

        openal_debug::check_errors();
    }

    void AlSource::pause() const {
        alSourcePause(m_source);

        openal_debug::check_errors();
    }

    void AlSource::resume() const {
        alSourcePlay(m_source);

        openal_debug::check_errors();
    }

    bool AlSource::is_playing() const {
        int state {};
        alGetSourcei(m_source, AL_SOURCE_STATE, &state);

        openal_debug::check_errors();

        return state == AL_PLAYING;
    }

    void AlSource::set_gain(float gain) const {
        assert(gain >= 0.0f);

        alSourcef(m_source, AL_GAIN, gain);

        openal_debug::check_errors();
    }

    void AlSource::set_pitch(float pitch) const {
        assert(pitch >= 0.0f);

        alSourcef(m_source, AL_PITCH, pitch);

        openal_debug::check_errors();
    }

    void AlSource::set_position(glm::vec3 position) const {
        alSource3f(m_source, AL_POSITION, position.x, position.y, position.z);

        openal_debug::check_errors();
    }

    void AlSource::set_velocity(glm::vec3 velocity) const {
        alSource3f(m_source, AL_VELOCITY, velocity.x, velocity.y, velocity.z);

        openal_debug::check_errors();
    }

    void AlSource::set_direction(glm::vec3 direction) const {
        alSource3f(m_source, AL_DIRECTION, direction.x, direction.y, direction.z);

        openal_debug::check_errors();
    }

    void AlSource::set_looping(bool looping) const {
        alSourcei(m_source, AL_LOOPING, static_cast<ALint>(looping));

        openal_debug::check_errors();
    }

    void AlSource::set_rolloff_factor(float rolloff_factor) const {
        alSourcef(m_source, AL_ROLLOFF_FACTOR, rolloff_factor);

        openal_debug::check_errors();
    }

    void AlSource::set_reference_distance(float reference_distance) const {
        alSourcef(m_source, AL_REFERENCE_DISTANCE, reference_distance);

        openal_debug::check_errors();
    }

    void AlSource::set_max_distance(float max_distance) const {
        alSourcef(m_source, AL_MAX_DISTANCE, max_distance);

        openal_debug::check_errors();
    }

    float AlSource::get_gain() const {
        float gain {};
        alGetSourcef(m_source, AL_GAIN, &gain);

        openal_debug::check_errors();

        return gain;
    }

    float AlSource::get_pitch() const {
        float pitch {};
        alGetSourcef(m_source, AL_PITCH, &pitch);

        openal_debug::check_errors();

        return pitch;
    }

    glm::vec3 AlSource::get_position() const {
        glm::vec3 position {};
        alGetSource3f(m_source, AL_POSITION, &position.x, &position.y, &position.z);

        openal_debug::check_errors();

        return position;
    }

    glm::vec3 AlSource::get_velocity() const {
        glm::vec3 velocity {};
        alGetSource3f(m_source, AL_VELOCITY, &velocity.x, &velocity.y, &velocity.z);

        openal_debug::check_errors();

        return velocity;
    }

    glm::vec3 AlSource::get_direction() const {
        glm::vec3 direction {};
        alGetSource3f(m_source, AL_DIRECTION, &direction.x, &direction.y, &direction.z);

        openal_debug::check_errors();

        return direction;
    }

    bool AlSource::get_looping() const {
        int looping {};
        alGetSourcei(m_source, AL_LOOPING, &looping);

        openal_debug::check_errors();

        return static_cast<bool>(looping);
    }

    float AlSource::get_rolloff_factor() const {
        float rolloff_factor {};
        alGetSourcef(m_source, AL_ROLLOFF_FACTOR, &rolloff_factor);

        openal_debug::check_errors();

        return rolloff_factor;
    }

    float AlSource::get_reference_distance() const {
        float reference_distance {};
        alGetSourcef(m_source, AL_REFERENCE_DISTANCE, &reference_distance);

        openal_debug::check_errors();

        return reference_distance;
    }

    float AlSource::get_max_distance() const {
        float max_distance {};
        alGetSourcef(m_source, AL_MAX_DISTANCE, &max_distance);

        openal_debug::check_errors();

        return max_distance;
    }

    void AlSource::play(AlBuffer* buffer) {
        stop();

        if (buffer->m_buffer != m_attached_buffer) {
            m_attached_buffer = buffer->m_buffer;
            buffer->m_sources_attached.insert(m_source);

            alSourcei(m_source, AL_BUFFER, m_attached_buffer);

            openal_debug::check_errors();
        }

        alSourcePlay(m_source);

        openal_debug::check_errors();
    }
}
