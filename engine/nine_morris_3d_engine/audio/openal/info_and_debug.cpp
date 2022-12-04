#include <AL/al.h>

#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/audio/openal/info_and_debug.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/exit.h"

namespace al {
    void maybe_check_errors() {
#ifdef PLATFORM_GAME_DEBUG
        const ALenum error = alGetError();

        if (error != AL_NO_ERROR) {
            std::cout << "OpenAL Debug Error:" << std::endl;

            switch(error) {
                case AL_INVALID_NAME:
                    REL_CRITICAL("({}) AL_INVALID_NAME: a bad name (ID) was passed to an OpenAL function", error);
                    break;
                case AL_INVALID_ENUM:
                    REL_CRITICAL("({}) AL_INVALID_ENUM: an invalid enum value was passed to an OpenAL function", error);
                    break;
                case AL_INVALID_VALUE:
                    REL_CRITICAL("({}) AL_INVALID_VALUE: an invalid value was passed to an OpenAL function", error);
                    break;
                case AL_INVALID_OPERATION:
                    REL_CRITICAL("({}) AL_INVALID_OPERATION: the requested operation is not valid", error);
                    break;
                case AL_OUT_OF_MEMORY:
                    REL_CRITICAL("({}) AL_OUT_OF_MEMORY: the requested operation resulted in OpenAL running out of memory", error);
                    break;
                default:
                    REL_CRITICAL("({}) Unknown AL error", error);
            }

            game_exit::exit_critical();
        }
#endif
    }

    std::string get_info() {
        std::string output;

        output.append("\n*** OpenAL Version ***\n");

        constexpr size_t LENGTH = 256;  // 256 should be enough

        char line[LENGTH];
        snprintf(line, LENGTH, "OpenAL version: %s\n", alGetString(AL_VERSION));
        output.append(line);

        return output;
    }

    const char* get_version() {
        return alGetString(AL_VERSION);
    }
}
