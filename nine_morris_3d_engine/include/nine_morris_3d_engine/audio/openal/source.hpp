#pragma once

#include <memory>

#include <glm/glm.hpp>

#include "nine_morris_3d_engine/audio/openal/buffer.hpp"

namespace sm {
    class AlSource {
    public:
        AlSource() noexcept;
        ~AlSource() noexcept;

        AlSource(const AlSource&) = delete;
        AlSource& operator=(const AlSource&) = delete;
        AlSource(AlSource&&) = delete;
        AlSource& operator=(AlSource&&) = delete;

        void play(std::shared_ptr<AlBuffer> buffer);
        void play(const std::unique_ptr<AlBuffer>& buffer);
        void stop() const noexcept;
        void pause() const noexcept;
        void resume() const noexcept;
        bool is_playing() const noexcept;

        void set_gain(float gain) const noexcept;
        void set_pitch(float pitch) const noexcept;
        void set_position(glm::vec3 position) const noexcept;
        void set_velocity(glm::vec3 velocity) const noexcept;
        void set_direction(glm::vec3 direction) const noexcept;
        void set_looping(bool looping) const noexcept;
        void set_rolloff_factor(float rolloff_factor) const noexcept;  // How fast the gain loses energy with distance
        void set_reference_distance(float reference_distance) const noexcept;  // The distance at which gain is 1.0
        void set_max_distance(float max_distance) const noexcept;  // The distance at which the gain is 0.0 (linear) or at which the gain doesn't decrease (clamped)

        float get_gain() const noexcept;
        float get_pitch() const noexcept;
        glm::vec3 get_position() const noexcept;
        glm::vec3 get_velocity() const noexcept;
        glm::vec3 get_direction() const noexcept;
        bool get_looping() const noexcept;
        float get_rolloff_factor() const noexcept;
        float get_reference_distance() const noexcept;
        float get_max_distance() const noexcept;
    private:
        void play(AlBuffer* buffer);

        unsigned int source {};
        unsigned int attached_buffer {};  // The buffer that is currently attached to this source
    };
}
