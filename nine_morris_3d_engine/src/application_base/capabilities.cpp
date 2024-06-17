#include "engine/application_base/capabilities.hpp"

#include <glad/glad.h>

#include "engine/other/logging.hpp"

namespace sm {
    int Capabilities::max_anisotropic_filtering_supported() {
        if (GLAD_GL_EXT_texture_filter_anisotropic) {
            float max_amount;
            glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY_EXT, &max_amount);

            return static_cast<int>(max_amount);
        } else {
            return 0;
        }
    }

    int Capabilities::max_samples_supported() {
        int max_samples;
        glGetIntegerv(GL_MAX_SAMPLES, &max_samples);

        int max_depth_texture_samples;
        glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &max_depth_texture_samples);

        int max_color_texture_samples;
        glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &max_color_texture_samples);

        if (max_depth_texture_samples < max_samples) {
            LOG_DIST_ERROR("GL_MAX_DEPTH_TEXTURE_SAMPLES < GL_MAX_SAMPLES");
        }

        if (max_color_texture_samples < max_samples) {
            LOG_DIST_ERROR("GL_MAX_COLOR_TEXTURE_SAMPLES < GL_MAX_SAMPLES");
        }

        return max_samples;
    }

    int Capabilities::max_texture_units_supported() {
        int max_units;
        glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &max_units);

        return max_units;
    }
}
