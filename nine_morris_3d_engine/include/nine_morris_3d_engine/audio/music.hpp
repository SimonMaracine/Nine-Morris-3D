#pragma once

#include <memory>

#include "nine_morris_3d_engine/audio/sound_data.hpp"

namespace sm {
    /*
        All music tracks are streamed, so they should be considered sound files longer than 1 minute.  // TODO do this!
        Sound effects should be sound files shorter than 45-50 seconds.
    */

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

        std::shared_ptr<AlSource> source;  // TODO maybe these don't need to be shared
        std::shared_ptr<AlBuffer> buffer;

        friend class internal::MusicPlayer;
    };

    namespace internal {
        class MusicPlayer {  // TODO revisit this class; rename things
        public:
            void play_music_track(std::shared_ptr<MusicTrack> music_track);
            void stop_music_track();  // Must be called before the OpenAL context is destroyed
            void pause_music_track();
            void continue_music_track();
            void set_music_gain(float gain);
        private:
            // Pointer is reset when music is stopped
            // Both are reset at the coresponding function call
            std::shared_ptr<MusicTrack> current_music_track;
            float current_gain {1.0f};
        };
    }
}
