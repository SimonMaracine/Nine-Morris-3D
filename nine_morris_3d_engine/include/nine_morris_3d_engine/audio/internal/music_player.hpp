#pragma once

#include <memory>

#include "nine_morris_3d_engine/audio/music_track.hpp"

namespace sm {
    class AlSource;
}

/*
    All music tracks are streamed, so they should be considered sound files longer than 1 minute.  // TODO do this!
    Sound effects should be sound files shorter than 45-50 seconds.
*/
namespace sm::internal {
    class MusicPlayer {
    public:
        MusicPlayer();

        // Must be called before the OpenAL context is destroyed
        void play(std::shared_ptr<MusicTrack> music_track);
        void stop();
        void pause();
        void resume();
        void set_gain(float gain);
    private:
        std::unique_ptr<AlSource> source;

        // Pointer is reset when music is stopped
        std::shared_ptr<MusicTrack> current_music_track;
    };
}
