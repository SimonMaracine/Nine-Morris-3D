#include "nine_morris_3d_engine/application/monitors.hpp"

#include <cassert>

#include <GLFW/glfw3.h>

#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    std::pair<int, int> Monitors::get_resolution(std::size_t index) const {
        assert(index < count);

        const GLFWvidmode* video_mode {glfwGetVideoMode(monitors[index])};

        if (video_mode == nullptr) {
            SM_THROW_ERROR(OtherError, "Could not get monitor video mode");
        }

        return std::make_pair(video_mode->width, video_mode->height);
    }

    std::pair<float, float> Monitors::get_content_scale(std::size_t index) const {
        assert(index < count);

        float xscale, yscale;
        glfwGetMonitorContentScale(monitors[index], &xscale, &yscale);

        return std::make_pair(xscale, yscale);
    }

    const char* Monitors::get_name(std::size_t index) const {
        assert(index < count);

        const char* name {glfwGetMonitorName(monitors[index])};

        if (name == nullptr) {
            SM_THROW_ERROR(OtherError, "Could not get monitor name");
        }

        return name;
    }

    std::size_t Monitors::get_count() const {
        return count;
    }
}
