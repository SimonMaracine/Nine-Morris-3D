#pragma once

#include <AL/al.h>

namespace al {
    class Source {
    public:
        Source();
        ~Source();

        void bind();
        static void unbind();
    private:
        ALuint source = 0;
    };
}
