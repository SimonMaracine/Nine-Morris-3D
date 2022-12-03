#pragma once

#include <AL/al.h>

#include "nine_morris_3d_engine/audio/openal/buffer.h"

namespace al {
    class Source {
    public:
        Source();
        ~Source();

        void play(Buffer* buffer);
        void stop();
    private:
        ALuint source = 0;
    };
}
