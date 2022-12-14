#pragma once

#include "nine_morris_3d_engine/audio/openal/source.h"
#include "nine_morris_3d_engine/audio/openal/buffer.h"
#include "nine_morris_3d_engine/audio/sound_data.h"
#include "nine_morris_3d_engine/other/encrypt.h"

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
        std::shared_ptr<al::Source> source;
        std::shared_ptr<al::Buffer> buffer;

        std::string name;

        friend void play_music_track(std::shared_ptr<MusicTrack> music_track);
        friend void stop_music_track();
        friend void pause_music_track();
        friend void continue_music_track();
    };

    void play_music_track(std::shared_ptr<MusicTrack> music_track);
    void stop_music_track();  // Must be called before the OpenAL context is destroyed
    void pause_music_track();
    void continue_music_track();  // TODO add more music API functions
}
