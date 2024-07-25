#include "nine_morris_3d_engine/audio/internal/context.hpp"

#include <AL/alc.h>

#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/listener.hpp"

namespace sm::internal {
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

        // Defaults
        listener::set_look_at_and_up(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        listener::set_gain(1.0f);
        listener::set_distance_model(listener::DistanceModel::InverseClamped);

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

    // AlListener& OpenAlContext::get_listener() {
    //     return listener;
    // }
}
