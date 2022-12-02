#pragma once

#include <AL/al.h>

namespace al {
    class Buffer {
    public:
        Buffer(size_t size, const void* data, int frequency);
        ~Buffer();
    private:
        ALuint buffer = 0;
    };
}
