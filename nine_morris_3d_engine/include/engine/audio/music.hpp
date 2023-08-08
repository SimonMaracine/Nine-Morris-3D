#pragma once

#include <memory>
#include <string>
#include <string_view>

#include "engine/audio/openal/source.hpp"
#include "engine/audio/openal/buffer.hpp"
#include "engine/audio/sound_data.hpp"
#include "engine/other/encrypt.hpp"

namespace sm {
    /*
        All music tracks are streamed, so they should be considered sound files longer than 1 minute.  // TODO do this!
        Sound effects should be sound files shorter than 45-50 seconds.
    */

    namespace music {  // FIXME make class
        class MusicTrack {
        public:
            MusicTrack(std::string_view file_path);
            MusicTrack(Encrypt::EncryptedFile file_path);
            MusicTrack(std::shared_ptr<SoundData> data);
            ~MusicTrack();

            MusicTrack(const MusicTrack&) = delete;
            MusicTrack& operator=(const MusicTrack&) = delete;
            MusicTrack(MusicTrack&&) = delete;
            MusicTrack& operator=(MusicTrack&&) = delete;
        private:
            void setup(std::shared_ptr<SoundData> data);

            std::shared_ptr<AlSource> source;
            std::shared_ptr<AlBuffer> buffer;

            std::string name;

            friend void play_music_track(std::shared_ptr<MusicTrack> music_track);
            friend void stop_music_track();
            friend void pause_music_track();
            friend void continue_music_track();
            friend void set_music_gain(float gain);
        };

        void uninitialize();
        void play_music_track(std::shared_ptr<MusicTrack> music_track);
        void stop_music_track();  // Must be called before the OpenAL context is destroyed
        void pause_music_track();
        void continue_music_track();
        void set_music_gain(float gain);
    }
}
