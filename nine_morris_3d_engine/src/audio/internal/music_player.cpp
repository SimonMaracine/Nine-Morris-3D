#include "nine_morris_3d_engine/audio/internal/music_player.hpp"

#include <cassert>

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/source.hpp"

namespace sm::internal {
    MusicPlayer::MusicPlayer(bool audio) {
        if (audio) {
            m_source = std::make_unique<AlSource>();
            m_source->set_rolloff_factor(0.0f);
            m_source->set_looping(true);
        }
    }

    void MusicPlayer::play(std::shared_ptr<MusicTrack> music_track) {
        m_current_music_track = music_track;

        m_source->play(m_current_music_track->m_buffer);

        LOG_DEBUG("Started playing music track");
    }

    void MusicPlayer::stop() {
        if (m_current_music_track == nullptr) {
            LOG_WARNING("No music track");
            return;
        }

        m_source->stop();

        LOG_DEBUG("Stopped playing music track");

        m_current_music_track = nullptr;
    }

    void MusicPlayer::pause() {
        if (m_current_music_track == nullptr) {
            LOG_WARNING("No music track");
            return;
        }

        m_source->pause();

        LOG_DEBUG("Paused playing music track");
    }

    void MusicPlayer::resume() {
        if (m_current_music_track == nullptr) {
            LOG_WARNING("No music track");
            return;
        }

        m_source->resume();

        LOG_DEBUG("Resumed playing music track");
    }

    void MusicPlayer::set_gain(float gain) {
        assert(gain >= 0.0f);

        if (gain > 1.0f) {
            LOG_WARNING("Gain is larger than 1.0");
        }

        m_source->set_gain(gain);
    }
}
