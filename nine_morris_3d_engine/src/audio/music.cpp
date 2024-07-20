#include "nine_morris_3d_engine/audio/music.hpp"

#include <cassert>

#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/source.hpp"
#include "nine_morris_3d_engine/audio/openal/buffer.hpp"

namespace sm {
    MusicTrack::MusicTrack(std::shared_ptr<SoundData> data) {
        setup(data);

        LOG_DEBUG("Loaded music track");
    }

    void MusicTrack::setup(std::shared_ptr<SoundData> data) {
        source = std::make_shared<AlSource>();
        buffer = std::make_shared<AlBuffer>(data);

        source->set_rolloff_factor(0.0f);
        source->set_looping(true);

        if (data->get_channels() != 2) {
            LOG_WARNING("Music track is not stereo");
        }
    }

    namespace internal {
        void MusicPlayer::play_music_track(std::shared_ptr<MusicTrack> music_track) {
            current_music_track = music_track;

            current_music_track->source->set_gain(current_gain);  // Set the gain for this potentially new music track
            current_music_track->source->play(current_music_track->buffer.get());

            LOG_DEBUG("Started playing music track");
        }

        void MusicPlayer::stop_music_track() {
            if (current_music_track == nullptr) {
                LOG_WARNING("No music track");
                return;
            }

            current_music_track->source->stop();

            LOG_DEBUG("Stopped playing music track");

            current_music_track = nullptr;
        }

        void MusicPlayer::pause_music_track() {
            if (current_music_track == nullptr) {
                LOG_WARNING("No music track");
                return;
            }

            current_music_track->source->pause();

            LOG_DEBUG("Paused playing music track");
        }

        void MusicPlayer::continue_music_track() {
            if (current_music_track == nullptr) {
                LOG_WARNING("No music track");
                return;
            }

            current_music_track->source->resume();

            LOG_DEBUG("Continued playing music track");
        }

        void MusicPlayer::set_music_gain(float gain) {
            assert(gain >= 0.0f);

            if (gain > 1.0f) {
                LOG_WARNING("Gain is larger than 1.0");
            }

            current_gain = gain;

            if (current_music_track != nullptr) {
                current_music_track->source->set_gain(gain);
            }
        }
    }
}
