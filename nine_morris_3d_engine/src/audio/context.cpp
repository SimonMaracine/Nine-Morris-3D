#include "nine_morris_3d_engine/audio/context.hpp"

#include <AL/alc.h>

#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    namespace internal {
        OpenAlContext::OpenAlContext(bool create)
            : create(create) {
            if (!create) {
                return;
            }

            // Choose the default device
            device = alcOpenDevice(nullptr);

            if (device == nullptr) {
                SM_THROW_ERROR(InitializationError, "Could not open an AL device");
            }

            context = alcCreateContext(device, nullptr);

            if (context == nullptr) {
                alcCloseDevice(device);

                SM_THROW_ERROR(InitializationError, "Could not create AL context");
            }

            if (alcMakeContextCurrent(context) == ALC_FALSE) {
                alcDestroyContext(context);
                alcCloseDevice(device);

                SM_THROW_ERROR(InitializationError, "Could not make AL context current");
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
}
