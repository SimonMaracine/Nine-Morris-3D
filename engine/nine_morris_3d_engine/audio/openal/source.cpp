#include <AL/al.h>

#include "nine_morris_3d_engine/audio/openal/source.h"
#include "nine_morris_3d_engine/other/logging.h"

namespace al {
    Source::Source() {
        alGenSources(1, &source);
        alSourcef(source, AL_GAIN, 1.0f);
        alSourcef(source, AL_PITCH, 1.0f);
        alSource3f(source, AL_POSITION, 0.0f, 0.0f, 0.0f);
        alSource3f(source, AL_VELOCITY, 0.0f, 0.0f, 0.0f);
        alSourcei(source, AL_LOOPING, AL_FALSE);

        DEB_DEBUG("Created AL source {}", source);
    }

    Source::~Source() {
        alDeleteSources(1, &source);

        DEB_DEBUG("Deleted AL source {}", source);
    }

    void Source::play(Buffer* buffer) {
        alSourcei(source, AL_BUFFER, buffer->buffer);
        alSourcePlay(source);
    }

    void Source::stop() {
        alSourceStop(source);
    }
}
