#include "nine_morris_3d_engine/audio/internal/context.hpp"

#include <AL/alc.h>

#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"
#include "nine_morris_3d_engine/audio/openal/listener.hpp"

namespace sm::internal {
    OpenAlContext::OpenAlContext(bool create)
        : m_create(create) {
        if (!m_create) {
            return;
        }

        // Choose the default device
        m_device = alcOpenDevice(nullptr);

        if (m_device == nullptr) {
            SM_THROW_ERROR(AudioError, "Could not open a playback device");
        }

        m_context = alcCreateContext(m_device, nullptr);

        if (m_context == nullptr) {
            alcCloseDevice(m_device);

            SM_THROW_ERROR(AudioError, "Could not create OpenAL context");
        }

        if (alcMakeContextCurrent(m_context) == ALC_FALSE) {
            alcDestroyContext(m_context);
            alcCloseDevice(m_device);

            SM_THROW_ERROR(AudioError, "Could not make OpenAL context current");
        }

        // Defaults
        listener::set_look_at_and_up(glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        listener::set_gain(1.0f);
        listener::set_distance_model(listener::DistanceModel::InverseClamped);

        LOG_INFO("Opened playback device and created OpenAL context");
    }

    OpenAlContext::~OpenAlContext() noexcept {
        if (!m_create) {
            return;
        }

        alcMakeContextCurrent(nullptr);
        alcDestroyContext(m_context);
        alcCloseDevice(m_device);

        LOG_INFO("Destroyed OpenAL context and closed playback device");
    }
}
