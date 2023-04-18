#include <AL/al.h>

#include "engine/application_base/platform.h"
#include "engine/audio/openal/info_and_debug.h"
#include "engine/other/logging.h"
#include "engine/application_base/panic.h"

namespace al {
    void maybe_check_errors() {
#ifdef NM3D_PLATFORM_DEBUG
        const ALenum error = alGetError();

        if (error != AL_NO_ERROR) {
            std::cout << "OpenAL Debug Error:" << std::endl;

            switch(error) {
                case AL_INVALID_NAME:
                    LOG_DIST_CRITICAL("({}) AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function", error);
                    break;
                case AL_INVALID_ENUM:
                    LOG_DIST_CRITICAL("({}) AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function", error);
                    break;
                case AL_INVALID_VALUE:
                    LOG_DIST_CRITICAL("({}) AL_INVALID_VALUE: an invalid value was passed to an OpenAL function", error);
                    break;
                case AL_INVALID_OPERATION:
                    LOG_DIST_CRITICAL("({}) AL_INVALID_OPERATION: the requested operation is not valid", error);
                    break;
                case AL_OUT_OF_MEMORY:
                    LOG_DIST_CRITICAL("({}) AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory", error);
                    break;
                default:
                    LOG_DIST_CRITICAL("({}) Unknown AL error", error);
            }

            panic::panic();
        }
#endif
    }

    std::string get_info() {
        std::string output;
        output.reserve(64);

        output.append("\n*** OpenAL Version ***\n");

        static constexpr size_t LENGTH = 256;  // 256 should be enough

        char line[LENGTH];
        snprintf(line, LENGTH, "OpenAL version: %s\n", alGetString(AL_VERSION));
        output.append(line);

        return output;
    }

    const char* get_version() {
        return alGetString(AL_VERSION);
    }
}
