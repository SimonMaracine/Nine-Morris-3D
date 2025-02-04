#include "nine_morris_3d_engine/audio/openal/debug.hpp"

#include <cstdio>

#include <AL/al.h>

#include "nine_morris_3d_engine/application/platform.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm::openal_debug {
    void check_errors() {
#ifndef SM_BUILD_DISTRIBUTION
        const ALenum error {alGetError()};

        if (error == AL_NO_ERROR) {
            return;
        }

        switch(error) {
            case AL_INVALID_NAME:
                LOG_CRITICAL("({}) OpenAL AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function", error);
                break;
            case AL_INVALID_ENUM:
                LOG_CRITICAL("({}) OpenAL AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function", error);
                break;
            case AL_INVALID_VALUE:
                LOG_CRITICAL("({}) OpenAL AL_INVALID_VALUE: an invalid value was passed to an OpenAL function", error);
                break;
            case AL_INVALID_OPERATION:
                LOG_CRITICAL("({}) OpenAL AL_INVALID_OPERATION: the requested operation is not valid", error);
                break;
            case AL_OUT_OF_MEMORY:
                LOG_CRITICAL("({}) OpenAL AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory", error);
                break;
            default:
                LOG_CRITICAL("({}) OpenAL: unknown error", error);
                break;
        }
#endif
    }

    std::string get_information() {
        std::string result;

        result += "*** OpenAL Driver ***\n";

        char buffer[256] {};  // Should be enough

        std::snprintf(buffer, sizeof(buffer), "OpenAL: %s\n", alGetString(AL_VERSION));
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "Renderer: %s\n", alGetString(AL_RENDERER));
        result += buffer;

        std::snprintf(buffer, sizeof(buffer), "Vendor: %s\n", alGetString(AL_VENDOR));
        result += buffer;

        return result;
    }

    const char* get_version() {
        return alGetString(AL_VERSION);
    }
}
