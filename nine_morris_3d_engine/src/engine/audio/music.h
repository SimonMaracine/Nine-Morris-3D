#pragma once

#include "engine/audio/openal/source.h"
#include "engine/audio/openal/buffer.h"
#include "engine/audio/sound_data.h"
#include "engine/other/encrypt.h"

/**
 * All music tracks are streamed, so they should be considered sound files longer than 1 minute.  // TODO do this!
 * Sound effects should be sound files shorter than 45-50 seconds.
 */
namespace music {
    class MusicTrack {
    public:
        MusicTrack(std::string_view file_path);
        MusicTrack(encrypt::EncryptedFile file_path);
        MusicTrack(std::shared_ptr<SoundData> data);
        ~MusicTrack();

        MusicTrack(const MusicTrack&) = delete;
        MusicTrack& operator=(const MusicTrack&) = delete;
        MusicTrack(MusicTrack&&) = delete;
        MusicTrack& operator=(MusicTrack&&) = delete;
    private:
        void setup(std::shared_ptr<SoundData> data);

        std::shared_ptr<al::Source> source;
        std::shared_ptr<al::Buffer> buffer;

        std::string name;

        friend void play_music_track(std::shared_ptr<MusicTrack> music_track);
        friend void stop_music_track();
        friend void pause_music_track();
        friend void continue_music_track();
        friend void set_music_gain(float gain);
    };

    void play_music_track(std::shared_ptr<MusicTrack> music_track);
    void stop_music_track();  // Must be called before the OpenAL context is destroyed
    void pause_music_track();
    void continue_music_track();  // TODO add more music API functions
    void set_music_gain(float gain);
}
