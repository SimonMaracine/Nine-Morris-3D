#include <AL/alc.h>

#include "engine/application_base/panic.hpp"
#include "engine/audio/context.hpp"
#include "engine/audio/openal/listener.hpp"
#include "engine/other/logging.hpp"

namespace sm {
    OpenAlContext::OpenAlContext() {
        // Choose the default device
        device = alcOpenDevice(nullptr);

        if (device == nullptr) {
            LOG_DIST_CRITICAL("Could not open an AL device");
            throw InitializationError;
        }

        context = alcCreateContext(device, nullptr);

        if (context == nullptr) {
            LOG_DIST_CRITICAL("Could not create AL context");
            throw InitializationError;
        }

        if (alcMakeContextCurrent(context) == ALC_FALSE) {
            alcDestroyContext(context);
            alcCloseDevice(device);

            LOG_DIST_CRITICAL("Could not make AL context current");
            throw InitializationError;
        }

        listener.set_distance_model(DistanceModel::InverseClamped);

        LOG_INFO("Opened OpenAL device and created context");
    }

    OpenAlContext::~OpenAlContext() {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);

        LOG_INFO("Destroyed OpenAL context and closed device");
    }
}
