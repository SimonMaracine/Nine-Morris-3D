#include "nine_morris_3d_engine/audio/openal/source.h"
#include "nine_morris_3d_engine/audio/openal/buffer.h"
#include "nine_morris_3d_engine/audio/music.h"
#include "nine_morris_3d_engine/audio/sound_data.h"
#include "nine_morris_3d_engine/other/encrypt.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/assert.h"

static std::shared_ptr<music::MusicTrack> _current_music_track = nullptr;
static float _gain = 1.0f;

namespace music {
    MusicTrack::MusicTrack(std::string_view file_path) {
        auto data = std::make_shared<SoundData>(file_path);

        setup(data);

        name = file_path;

        DEB_DEBUG("Loaded music track `{}`", name);
    }

    MusicTrack::MusicTrack(encrypt::EncryptedFile file_path) {
        auto data = std::make_shared<SoundData>(file_path);

        setup(data);

        name = file_path;

        DEB_DEBUG("Loaded music track `{}`", name);
    }

    MusicTrack::MusicTrack(std::shared_ptr<SoundData> data) {
        setup(data);

        name = data->get_file_path();

        DEB_DEBUG("Loaded music track `{}`", name);
    }

    MusicTrack::~MusicTrack() {
        DEB_DEBUG("Unloaded music track `{}`", name);
    }

    void MusicTrack::setup(std::shared_ptr<SoundData> data) {
        source = std::make_shared<al::Source>();
        buffer = std::make_shared<al::Buffer>(data);

        source->set_rolloff_factor(0.0f);

        if (data->get_channels() != 2) {
            DEB_WARNING("Music track is not stereo");
        }
    }

    void play_music_track(std::shared_ptr<MusicTrack> music_track) {
        _current_music_track = music_track;

        _current_music_track->source->set_gain(_gain);  // Set the gain for this potentially new music track
        _current_music_track->source->play(_current_music_track->buffer.get());

        DEB_DEBUG("Started playing music track `{}`", _current_music_track->name);
    }

    void stop_music_track() {
        if (_current_music_track == nullptr) {
            DEB_WARNING("No music track pointer");
            return;
        }

        _current_music_track->source->stop();

        DEB_DEBUG("Stopped playing music track `{}`", _current_music_track->name);

        _current_music_track = nullptr;
    }

    void pause_music_track() {
        if (_current_music_track == nullptr) {
            DEB_WARNING("No music track pointer");
            return;
        }

        _current_music_track->source->pause();

        DEB_DEBUG("Paused playing music track `{}`", _current_music_track->name);
    }

    void continue_music_track() {
        if (_current_music_track == nullptr) {
            DEB_WARNING("No music track pointer");
            return;
        }

        _current_music_track->source->continue_();

        DEB_DEBUG("Continued playing music track `{}`", _current_music_track->name);
    }

    void set_music_gain(float gain) {
        ASSERT(gain >= 0.0f, "Gain must be positive");

        if (gain > 1.0f) {
            DEB_WARNING("Gain is larger than 1.0");
        }

        _gain = gain;

        if (_current_music_track != nullptr) {
            _current_music_track->source->set_gain(gain);
        }
    }
}
