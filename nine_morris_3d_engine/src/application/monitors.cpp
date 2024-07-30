#include "nine_morris_3d_engine/application/monitors.hpp"

#include <cassert>

#include <GLFW/glfw3.h>

#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    std::pair<int, int> Monitors::get_resolution(std::size_t index) const {
        assert(index < m_count);

        const GLFWvidmode* video_mode {glfwGetVideoMode(m_monitors[index])};

        if (video_mode == nullptr) {
            SM_THROW_ERROR(OtherError, "Could not get monitor video mode");
        }

        return std::make_pair(video_mode->width, video_mode->height);
    }

    std::pair<float, float> Monitors::get_content_scale(std::size_t index) const noexcept {
        assert(index < m_count);

        float xscale, yscale;
        glfwGetMonitorContentScale(m_monitors[index], &xscale, &yscale);

        return std::make_pair(xscale, yscale);
    }

    const char* Monitors::get_name(std::size_t index) const {
        assert(index < m_count);

        const char* name {glfwGetMonitorName(m_monitors[index])};

        if (name == nullptr) {
            SM_THROW_ERROR(OtherError, "Could not get monitor name");
        }

        return name;
    }

    std::size_t Monitors::get_count() const noexcept {
        return m_count;
    }
}
