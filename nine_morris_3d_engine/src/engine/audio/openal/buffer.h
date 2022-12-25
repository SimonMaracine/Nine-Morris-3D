#pragma once

#include <AL/al.h>

#include "engine/audio/sound_data.h"

namespace al {
    class Buffer {
    public:
        Buffer(const void* data, size_t size, int channels, size_t bps, int frequency);
        Buffer(std::shared_ptr<SoundData> data);
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        Buffer(Buffer&&) = delete;
        Buffer& operator=(Buffer&&) = delete;
    private:
        ALuint buffer = 0;
        std::unordered_set<ALuint> sources_attached;  // The sources that this buffer is attached to

        friend class Source;
    };
}
