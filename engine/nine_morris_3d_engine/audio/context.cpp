// #include <AL/al.h>
#include <AL/alc.h>

#include "nine_morris_3d_engine/audio/context.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/exit.h"

static void get_available_devices() {
    ALCdevice* device;
    const ALCchar* devices = alcGetString(device, ALC_DEVICE_SPECIFIER);

    // TODO implement
}

OpenALContext::OpenALContext() {
    // Choose the default device
    device = alcOpenDevice(nullptr);

    if (device == nullptr) {
        REL_CRITICAL("Could not open an AL device, exiting...");
        game_exit::exit_critical();
    }

    context = alcCreateContext(device, nullptr);  // TODO context attributes

    if (context == nullptr) {
        REL_CRITICAL("Could not create an AL context, exiting...");
        game_exit::exit_critical();
    }

    if (alcMakeContextCurrent(context) == ALC_FALSE) {  // FIXME clean up gracefully
        REL_CRITICAL("Could not make AL context current, exiting...");
        game_exit::exit_critical();
    }
}

OpenALContext::~OpenALContext() {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(context);
    alcCloseDevice(device);
}
