#include "nine_morris_3d_engine/audio/music.h"

namespace music {
    static std::shared_ptr<MusicTrack> _current_music_track = nullptr;

    MusicTrack::MusicTrack(std::string_view file_path) {

    }

    MusicTrack::~MusicTrack() {

    }

    void play_music_track(std::shared_ptr<MusicTrack> music_track) {
        _current_music_track = music_track;
        // TODO this
    }

    void stop_music_track() {
        // TODO this
        _current_music_track = nullptr;
    }

    void pause_music_track() {

    }

    void continue_music_track() {

    }
}
