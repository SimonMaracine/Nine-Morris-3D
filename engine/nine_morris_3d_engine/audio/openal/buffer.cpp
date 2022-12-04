#include <AL/al.h>

#include "nine_morris_3d_engine/audio/openal/buffer.h"
#include "nine_morris_3d_engine/audio/openal/info_and_debug.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/exit.h"

static ALenum get_format(int channels, size_t bps) {
    ALenum format = 0;

    if (channels == 1 && bps == 8) {
        format = AL_FORMAT_MONO8;
    } else if (channels == 1 && bps == 16) {
        format = AL_FORMAT_MONO16;
    } else if (channels == 2 && bps == 8) {
        format = AL_FORMAT_STEREO8;
    } else if (channels == 2 && bps == 16) {
        format = AL_FORMAT_STEREO16;
    } else {
        REL_CRITICAL("Unknown format: channels = `{}`, bps = `{}`, exiting...", channels, bps);
        game_exit::exit_critical();
    }

    return format;
}

namespace al {
    Buffer::Buffer(const void* data, size_t size, int channels, size_t bps, int frequency) {
        alGenBuffers(1, &buffer);
        alBufferData(buffer, get_format(channels, bps), data, size, frequency);

        maybe_check_errors();

        DEB_DEBUG("Created AL buffer {}", buffer);
    }

    Buffer::Buffer(std::shared_ptr<SoundData> data) {
        alGenBuffers(1, &buffer);
        alBufferData(
            buffer,
            get_format(data->get_channels(), data->get_bps()),
            data->get_data(),
            data->get_size(),
            data->get_frequency()
        );

        maybe_check_errors();

        DEB_DEBUG("Created AL buffer {}", buffer);
    }

    Buffer::~Buffer() {
        if (source_attached != 0) {
            alSourceStop(source_attached);
            alSourcei(source_attached, AL_BUFFER, 0);

            maybe_check_errors();
        }

        alDeleteBuffers(1, &buffer);

        maybe_check_errors();

        DEB_DEBUG("Deleted AL buffer {}", buffer);
    }
}
