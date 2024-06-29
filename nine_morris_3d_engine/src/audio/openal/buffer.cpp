#include "nine_morris_3d_engine/audio/openal/buffer.hpp"

#include <AL/al.h>

#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/debug.hpp"

namespace sm {
    static ALenum get_format(int channels, std::size_t bps) {
        ALenum format {};

        if (channels == 1 && bps == 8) {
            format = AL_FORMAT_MONO8;
        } else if (channels == 1 && bps == 16) {
            format = AL_FORMAT_MONO16;
        } else if (channels == 2 && bps == 8) {
            format = AL_FORMAT_STEREO8;
        } else if (channels == 2 && bps == 16) {
            format = AL_FORMAT_STEREO16;
        } else {
            SM_CRITICAL_ERROR(RuntimeError::ResourceLoading, "Unknown format: channels = {}, bps = {}", channels, bps);
        }

        return format;
    }

    AlBuffer::AlBuffer(const void* data, std::size_t size, int channels, std::size_t bps, int frequency) {
        alGenBuffers(1, &buffer);
        alBufferData(buffer, get_format(channels, bps), data, static_cast<int>(size), frequency);

        openal_debug::check_errors();

        LOG_DEBUG("Created AL buffer {}", buffer);
    }

    AlBuffer::AlBuffer(std::shared_ptr<SoundData> data) {
        alGenBuffers(1, &buffer);
        alBufferData(
            buffer,
            get_format(data->get_channels(), data->get_bps()),
            data->get_data(),
            static_cast<int>(data->get_size()),
            data->get_frequency()
        );

        openal_debug::check_errors();

        LOG_DEBUG("Created AL buffer {}", buffer);
    }

    AlBuffer::~AlBuffer() {
        for (const ALuint source : sources_attached) {
            alSourceStop(source);
            alSourcei(source, AL_BUFFER, 0);

            openal_debug::check_errors();
        }

        alDeleteBuffers(1, &buffer);

        openal_debug::check_errors();

        LOG_DEBUG("Deleted AL buffer {}", buffer);
    }
}
