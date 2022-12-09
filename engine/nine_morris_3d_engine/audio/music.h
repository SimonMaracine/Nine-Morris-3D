#pragma once

#include "nine_morris_3d_engine/audio/openal/source.h"

namespace music {
    class MusicTrack {
    public:
        MusicTrack(std::string_view file_path);
        ~MusicTrack();

        MusicTrack(const MusicTrack&) = delete;
        MusicTrack& operator=(const MusicTrack&) = delete;
        MusicTrack(MusicTrack&&) = delete;
        MusicTrack& operator=(MusicTrack&&) = delete;
    private:
        std::shared_ptr<al::Source> source;
        std::shared_ptr<al::Buffer> buffer;
    };

    void play_music_track(std::shared_ptr<MusicTrack> music_track);
    void stop_music_track();
    void pause_music_track();
    void continue_music_track();
}
