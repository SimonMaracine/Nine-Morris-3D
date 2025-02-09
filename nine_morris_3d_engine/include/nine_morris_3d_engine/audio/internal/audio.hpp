#pragma once

#include <memory>

#include "nine_morris_3d_engine/audio/sound_data.hpp"

namespace sm::internal {
    namespace audio {
        // Initialize the audio system
        // Must be called after SDL initialization
        void initialize();

        // Uninitialize the audio system
        // After this, no sound should be played
        void uninitialize();

        // Play a short sound once
        void play_sound(std::shared_ptr<SoundData> sound_data);

        // Set the master volume of sounds (0.0 -> 1.0)
        void set_volume(float volume);
    }
}
