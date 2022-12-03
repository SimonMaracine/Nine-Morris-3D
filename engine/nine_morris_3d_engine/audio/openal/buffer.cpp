#include <AL/al.h>

#include "nine_morris_3d_engine/audio/openal/buffer.h"
#include "nine_morris_3d_engine/other/logging.h"

namespace al {
    Buffer::Buffer(const void* data, size_t size, int frequency) {
        alGenBuffers(1, &buffer);
        alBufferData(buffer, AL_FORMAT_MONO16, data, size, frequency);

        DEB_DEBUG("Created AL buffer {}", buffer);
    }

    Buffer::~Buffer() {
        alDeleteBuffers(1, &buffer);

        DEB_DEBUG("Deleted AL buffer {}", buffer);
    }
}
