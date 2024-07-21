#pragma once

#include <memory>

#include "nine_morris_3d_engine/audio/sound_data.hpp"

namespace sm {
    class AlSource;
    class AlBuffer;

    namespace internal {
        class MusicPlayer;
    }

    class MusicTrack {
    public:
        explicit MusicTrack(std::shared_ptr<SoundData> data);
    private:
        void setup(std::shared_ptr<SoundData> data);

        // TODO maybe source should be in music player
        std::shared_ptr<AlSource> source;  // TODO maybe these don't need to be shared
        std::shared_ptr<AlBuffer> buffer;

        friend class internal::MusicPlayer;
    };
}
