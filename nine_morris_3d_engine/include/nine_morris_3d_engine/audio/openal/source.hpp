#pragma once

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/audio/openal/buffer.hpp"

namespace sm {
    class AlSource {
    public:
        AlSource();
        ~AlSource();

        AlSource(const AlSource&) = delete;
        AlSource& operator=(const AlSource&) = delete;
        AlSource(AlSource&&) = delete;
        AlSource& operator=(AlSource&&) = delete;

        void play(AlBuffer* buffer);
        void stop() const;
        void pause() const;
        void resume() const;
        bool is_playing() const;

        void set_gain(float gain) const;
        void set_pitch(float pitch) const;
        void set_position(glm::vec3 position) const;
        void set_velocity(glm::vec3 velocity) const;
        void set_direction(glm::vec3 direction) const;
        void set_looping(bool looping) const;
        void set_rolloff_factor(float rolloff_factor) const;  // How fast the gain loses energy with distance
        void set_reference_distance(float reference_distance) const;  // The distance at which gain is 1.0
        void set_max_distance(float max_distance) const;  // The distance at which the gain is 0.0 (linear) or at which the gain doesn't decrease (clamped)

        float get_gain() const;
        float get_pitch() const;
        glm::vec3 get_position() const;
        glm::vec3 get_velocity() const;
        glm::vec3 get_direction() const;
        bool get_looping() const;
        float get_rolloff_factor() const;
        float get_reference_distance() const;
        float get_max_distance() const;
    private:
        unsigned int source {};
        unsigned int attached_buffer {};  // The buffer that is currently attached to this source
    };
}
