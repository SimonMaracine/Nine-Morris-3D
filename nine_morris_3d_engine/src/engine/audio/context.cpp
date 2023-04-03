#include <AL/alc.h>

#include "engine/application/platform.h"
#include "engine/audio/context.h"
#include "engine/audio/openal/listener.h"
#include "engine/other/logging.h"
#include "engine/application/panic.h"

static ALCdevice* _global_device = nullptr;
static ALCcontext* _global_context = nullptr;

#if 0
static void maybe_check_errors(ALCdevice* device) {
#ifdef NM3D_PLATFORM_DEBUG
    const ALCenum error = alcGetError(device);

    if (error != ALC_NO_ERROR) {
        LOG_DIST_CRITICAL("OpenAL Context Debug Error: {}", error);
        panic::panic();
    }
#endif
}
#endif

OpenAlContext::OpenAlContext() {
    // Choose the default device
    device = alcOpenDevice(nullptr);

    if (device == nullptr) {
        LOG_DIST_CRITICAL("Could not open an AL device, exiting...");
        panic::panic();
    }

    context = alcCreateContext(device, nullptr);

    if (context == nullptr) {
        LOG_DIST_CRITICAL("Could not create AL context, exiting...");
        panic::panic();
    }

    if (alcMakeContextCurrent(context) == ALC_FALSE) {
        alcDestroyContext(context);
        alcCloseDevice(device);

        LOG_DIST_CRITICAL("Could not make AL context current, exiting...");
        panic::panic();
    }

    _global_device = device;
    _global_context = context;

    listener.set_distance_model(al::DistanceModel::InverseClamped);

    LOG_INFO("Created OpenAL device and context");
}

OpenAlContext::~OpenAlContext() {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);

    _global_device = nullptr;
    _global_context = nullptr;

    LOG_INFO("Destroyed OpenAL context and device");
}

void destroy_openal_context() {
    alcMakeContextCurrent(nullptr);

    if (_global_context != nullptr) {
        alcDestroyContext(_global_context);

        if (_global_device != nullptr) {
            alcCloseDevice(_global_device);
        }
    }
}
