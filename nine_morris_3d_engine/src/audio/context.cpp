#include "engine/audio/context.hpp"

#include <AL/alc.h>

#include "engine/application_base/error.hpp"
#include "engine/other/logging.hpp"

namespace sm {
    OpenAlContext::OpenAlContext(bool create)
        : create(create) {
        if (!create) {
            return;
        }

        // Choose the default device
        device = alcOpenDevice(nullptr);

        if (device == nullptr) {
            LOG_DIST_CRITICAL("Could not open an AL device");
            throw RuntimeError::Initialization;
        }

        context = alcCreateContext(device, nullptr);

        if (context == nullptr) {
            LOG_DIST_CRITICAL("Could not create AL context");
            throw RuntimeError::Initialization;
        }

        if (alcMakeContextCurrent(context) == ALC_FALSE) {
            alcDestroyContext(context);
            alcCloseDevice(device);

            LOG_DIST_CRITICAL("Could not make AL context current");
            throw RuntimeError::Initialization;
        }

        listener.set_distance_model(DistanceModel::InverseClamped);

        LOG_INFO("Opened OpenAL device and created context");
    }

    OpenAlContext::~OpenAlContext() {
        if (!create) {
            return;
        }

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);

        LOG_INFO("Destroyed OpenAL context and closed device");
    }
}
