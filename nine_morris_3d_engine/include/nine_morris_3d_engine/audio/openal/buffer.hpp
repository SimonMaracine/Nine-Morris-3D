#pragma once

#include <memory>
#include <cstddef>
#include <unordered_set>

#include "nine_morris_3d_engine/audio/sound_data.hpp"

namespace sm {
    class AlSource;

    class AlBuffer {
    public:
        AlBuffer(const void* data, std::size_t size, int channels, std::size_t bps, int frequency);
        explicit AlBuffer(std::shared_ptr<SoundData> data);
        ~AlBuffer();

        AlBuffer(const AlBuffer&) = delete;
        AlBuffer& operator=(const AlBuffer&) = delete;
        AlBuffer(AlBuffer&&) = delete;
        AlBuffer& operator=(AlBuffer&&) = delete;
    private:
        unsigned int buffer {};
        std::unordered_set<unsigned int> sources_attached;  // The sources that this buffer is attached to

        friend class AlSource;
    };
}
