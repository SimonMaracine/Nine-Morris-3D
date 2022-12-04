#pragma once

#include <AL/al.h>

#include "nine_morris_3d_engine/audio/sound_data.h"

namespace al {
    class Buffer {
    public:
        Buffer(const void* data, size_t size, int channels, size_t bps, int frequency);
        Buffer(std::shared_ptr<SoundData> data);
        ~Buffer();
    private:
        ALuint buffer = 0;
        ALuint source_attached = 0;

        friend class Source;
    };
}
