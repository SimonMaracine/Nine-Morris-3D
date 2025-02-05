#pragma once

#include <memory>

#include "nine_morris_3d_engine/audio/sound_data.hpp"

namespace sm::internal {
    namespace audio {
        void initialize();
        void uninitialize();

        void play_sound(std::shared_ptr<SoundData> sound_data);
        void set_volume(float volume);
    }
}
