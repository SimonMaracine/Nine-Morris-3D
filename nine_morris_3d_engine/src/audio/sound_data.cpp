#include "nine_morris_3d_engine/audio/sound_data.hpp"

#include <SDL3/SDL.h>
#include <SDL3_mixer/SDL_mixer.h>

#include "nine_morris_3d_engine/application/internal/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    SoundData::SoundData(const std::filesystem::path& file_path) {
        Mix_Chunk* data {Mix_LoadWAV(file_path.string().c_str())};

        if (data == nullptr) {
            SM_THROW_ERROR(internal::ResourceError, "Could not load sound data");
        }

        m_data = data;

        LOG_DEBUG("Loaded sound data");
    }

    SoundData::~SoundData() {
        Mix_FreeChunk(static_cast<Mix_Chunk*>(m_data));

        LOG_DEBUG("Freed sound data");
    }
}
