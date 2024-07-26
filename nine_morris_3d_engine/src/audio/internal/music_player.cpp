#include "nine_morris_3d_engine/audio/internal/music_player.hpp"

#include <cassert>

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/source.hpp"

namespace sm::internal {
    MusicPlayer::MusicPlayer()
        : source(std::make_unique<AlSource>()) {
        source->set_rolloff_factor(0.0f);
        source->set_looping(true);
    }

    void MusicPlayer::play(std::shared_ptr<MusicTrack> music_track) {
        current_music_track = music_track;

        source->play(current_music_track->buffer);

        LOG_DEBUG("Started playing music track");
    }

    void MusicPlayer::stop() {
        if (current_music_track == nullptr) {
            LOG_WARNING("No music track");
            return;
        }

        source->stop();

        LOG_DEBUG("Stopped playing music track");

        current_music_track = nullptr;
    }

    void MusicPlayer::pause() {
        if (current_music_track == nullptr) {
            LOG_WARNING("No music track");
            return;
        }

        source->pause();

        LOG_DEBUG("Paused playing music track");
    }

    void MusicPlayer::resume() {
        if (current_music_track == nullptr) {
            LOG_WARNING("No music track");
            return;
        }

        source->resume();

        LOG_DEBUG("Resumed playing music track");
    }

    void MusicPlayer::set_gain(float gain) {
        assert(gain >= 0.0f);

        if (gain > 1.0f) {
            LOG_WARNING("Gain is larger than 1.0");
        }

        source->set_gain(gain);
    }
}
