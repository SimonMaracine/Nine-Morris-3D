#include "nine_morris_3d_engine/audio/internal/audio.hpp"

#include <cassert>

#include <SDL3_mixer/SDL_mixer.h>

#include "nine_morris_3d_engine/application/internal/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm::internal {
    void audio::initialize() {
        if (Mix_Init(MIX_INIT_OGG) != MIX_INIT_OGG) {
            SM_THROW_ERROR(AudioError, "Could not initialize audio");
        }

        SDL_AudioSpec specification;
        specification.format = SDL_AUDIO_S16;
        specification.channels = 1;
        specification.freq = 44100;

        if (!Mix_OpenAudio(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, &specification)) {
            SM_THROW_ERROR(AudioError, "Could not open audio device");
        }

        LOG_INFO("Initialized audio");
    }

    void audio::uninitialize() {
        Mix_CloseAudio();
        Mix_Quit();

        LOG_INFO("Uninitialized audio");
    }

    void audio::play_sound(std::shared_ptr<SoundData> sound_data) {
        if (Mix_PlayChannel(-1, static_cast<Mix_Chunk*>(sound_data->get_data()), 0) < 0) {
            LOG_WARNING("Could not play sound");
        }
    }

    void audio::set_volume(float volume) {
        assert(volume >= 0);

        volume = utils::map(volume, 0.0f, 1.0f, 0.0f, static_cast<float>(MIX_MAX_VOLUME));

        Mix_Volume(-1, static_cast<int>(volume));
    }
}
