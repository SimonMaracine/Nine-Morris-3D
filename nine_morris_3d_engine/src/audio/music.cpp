#include <memory>
#include <string_view>

#include "engine/audio/openal/source.hpp"
#include "engine/audio/openal/buffer.hpp"
#include "engine/audio/music.hpp"
#include "engine/audio/sound_data.hpp"
#include "engine/other/encrypt.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/assert.hpp"

namespace sm {
    // Pointer is reset when music is stopped
    // Both are reset at the coresponding function call
    static std::shared_ptr<music::MusicTrack> g_current_music_track = nullptr;
    static float g_current_gain = 1.0f;

    namespace music {
        MusicTrack::MusicTrack(std::string_view file_path) {
            const auto data = std::make_shared<SoundData>(file_path);

            setup(data);

            name = file_path;

            LOG_DEBUG("Loaded music track `{}`", name);
        }

        MusicTrack::MusicTrack(Encrypt::EncryptedFile file_path) {
            const auto data = std::make_shared<SoundData>(file_path);

            setup(data);

            name = file_path;

            LOG_DEBUG("Loaded music track `{}`", name);
        }

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

        void uninitialize() {
            stop_music_track();
            g_current_gain = 1.0f;

            LOG_INFO("Uninitialized music");
        }

        void play_music_track(std::shared_ptr<MusicTrack> music_track) {
            g_current_music_track = music_track;

            g_current_music_track->source->set_gain(g_current_gain);  // Set the gain for this potentially new music track
            g_current_music_track->source->play(g_current_music_track->buffer.get());

            LOG_DEBUG("Started playing music track `{}`", g_current_music_track->name);
        }

        void stop_music_track() {
            if (g_current_music_track == nullptr) {
                LOG_WARNING("No music track pointer");
                return;
            }

            g_current_music_track->source->stop();

            LOG_DEBUG("Stopped playing music track `{}`", g_current_music_track->name);

            g_current_music_track = nullptr;
        }

        void pause_music_track() {
            if (g_current_music_track == nullptr) {
                LOG_WARNING("No music track pointer");
                return;
            }

            g_current_music_track->source->pause();

            LOG_DEBUG("Paused playing music track `{}`", g_current_music_track->name);
        }

        void continue_music_track() {
            if (g_current_music_track == nullptr) {
                LOG_WARNING("No music track pointer");
                return;
            }

            g_current_music_track->source->continue_();

            LOG_DEBUG("Continued playing music track `{}`", g_current_music_track->name);
        }

        void set_music_gain(float gain) {
            ASSERT(gain >= 0.0f, "Gain must be positive");

            if (gain > 1.0f) {
                LOG_WARNING("Gain is larger than 1.0");
            }

            g_current_gain = gain;

            if (g_current_music_track != nullptr) {
                g_current_music_track->source->set_gain(gain);
            }
        }
    }
}
