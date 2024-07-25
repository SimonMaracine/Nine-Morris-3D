#pragma once

#include <memory>

#include "nine_morris_3d_engine/audio/sound_data.hpp"

namespace sm {
    class AlBuffer;

    namespace internal {
        class MusicPlayer;
    }

    class MusicTrack {
    public:
        explicit MusicTrack(std::shared_ptr<SoundData> data);
    private:
        std::unique_ptr<AlBuffer> buffer;

        friend class internal::MusicPlayer;
    };
}
