#pragma once

#include <AL/al.h>

namespace al {
    class Buffer {
    public:
        Buffer(const void* data, size_t size, int frequency);
        ~Buffer();
    private:
        ALuint buffer = 0;

        friend class Source;
    };
}
