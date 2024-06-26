#include "engine/audio/openal/debug.hpp"

#include <cstdio>
#include <cstdlib>

#include <AL/al.h>

#include "engine/application_base/platform.hpp"
#include "engine/application_base/logging.hpp"

namespace sm {
    void openal_debug::check_errors() {
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

        std::abort();
#endif
    }

    std::string openal_debug::get_information() {
        static constexpr std::size_t BUFFER_LENGTH {256};  // 256 should be enough

        char buffer[BUFFER_LENGTH] {};
        std::string result;

        result += "*** OpenAL Version ***\n";

        std::snprintf(buffer, BUFFER_LENGTH, "OpenAL version: %s\n", alGetString(AL_VERSION));
        result += buffer;

        return result;
    }

    const char* openal_debug::get_version() {
        return alGetString(AL_VERSION);
    }
}
