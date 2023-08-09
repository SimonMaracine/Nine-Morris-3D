#include <AL/alc.h>

#include "engine/application_base/panic.hpp"
#include "engine/audio/context.hpp"
#include "engine/audio/openal/listener.hpp"
#include "engine/other/logging.hpp"

namespace sm {
    static ALCdevice* global_device = nullptr;
    static ALCcontext* global_context = nullptr;

    OpenAlContext::OpenAlContext() {
        // Choose the default device
        device = alcOpenDevice(nullptr);

        if (device == nullptr) {
            LOG_DIST_CRITICAL("Could not open an AL device");
            panic();
        }

        context = alcCreateContext(device, nullptr);

        if (context == nullptr) {
            LOG_DIST_CRITICAL("Could not create AL context");
            panic();
        }

        if (alcMakeContextCurrent(context) == ALC_FALSE) {
            alcDestroyContext(context);
            alcCloseDevice(device);

            LOG_DIST_CRITICAL("Could not make AL context current");
            panic();
        }

        global_device = device;
        global_context = context;

        listener.set_distance_model(DistanceModel::InverseClamped);

        LOG_INFO("Created OpenAL device and context");
    }

    OpenAlContext::~OpenAlContext() {
        alcMakeContextCurrent(nullptr);
        alcDestroyContext(context);
        alcCloseDevice(device);

        global_device = nullptr;
        global_context = nullptr;

        LOG_INFO("Destroyed OpenAL context and device");
    }

    void OpenAlContext::destroy_openal_context() {
        alcMakeContextCurrent(nullptr);

        if (global_context != nullptr) {
            alcDestroyContext(global_context);

            if (global_device != nullptr) {
                alcCloseDevice(global_device);
            }
        }

        // Global device and context pointers don't need to be reset
    }
}
