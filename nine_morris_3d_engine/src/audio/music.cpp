#include <memory>
#include <string>

#include "engine/audio/openal/source.hpp"
#include "engine/audio/openal/buffer.hpp"
#include "engine/audio/music.hpp"
#include "engine/audio/sound_data.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    MusicTrack::MusicTrack(const std::string& file_path) {
        const auto data {std::make_shared<SoundData>(file_path)};

        setup(data);

        name = file_path;

        LOG_DEBUG("Loaded music track `{}`", name);
    }

    // MusicTrack::MusicTrack(const EncrFile& file_path) {
    //     const auto data {std::make_shared<SoundData>(file_path)};

    //     setup(data);

    //     name = file_path;

    //     LOG_DEBUG("Loaded music track `{}`", name);
    // }

    MusicTrack::MusicTrack(std::shared_ptr<SoundData> data) {
        setup(data);

        name = data->get_file_path();

        LOG_DEBUG("Loaded music track `{}`", name);
    }

    MusicTrack::~MusicTrack() {
        LOG_DEBUG("Unloaded music track `{}`", name);
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

    void MusicPlayer::uninitialize() {
        stop_music_track();
        current_gain = 1.0f;

        LOG_INFO("Uninitialized music");
    }

    void MusicPlayer::play_music_track(std::shared_ptr<MusicTrack> music_track) {
        current_music_track = music_track;

        current_music_track->source->set_gain(current_gain);  // Set the gain for this potentially new music track
        current_music_track->source->play(current_music_track->buffer.get());

        LOG_DEBUG("Started playing music track `{}`", current_music_track->name);
    }

    void MusicPlayer::stop_music_track() {
        if (current_music_track == nullptr) {
            LOG_WARNING("No music track");
            return;
        }

        current_music_track->source->stop();

        LOG_DEBUG("Stopped playing music track `{}`", current_music_track->name);

        current_music_track = nullptr;
    }

    void MusicPlayer::pause_music_track() {
        if (current_music_track == nullptr) {
            LOG_WARNING("No music track");
            return;
        }

        current_music_track->source->pause();

        LOG_DEBUG("Paused playing music track `{}`", current_music_track->name);
    }

    void MusicPlayer::continue_music_track() {
        if (current_music_track == nullptr) {
            LOG_WARNING("No music track");
            return;
        }

        current_music_track->source->resume();

        LOG_DEBUG("Continued playing music track `{}`", current_music_track->name);
    }

    void MusicPlayer::set_music_gain(float gain) {
        SM_ASSERT(gain >= 0.0f, "Gain must be positive");

        if (gain > 1.0f) {
            LOG_WARNING("Gain is larger than 1.0");
        }

        current_gain = gain;

        if (current_music_track != nullptr) {
            current_music_track->source->set_gain(gain);
        }
    }

    std::shared_ptr<MusicTrack> MusicPlayer::current_music_track {nullptr};
    float MusicPlayer::current_gain {1.0f};
}
