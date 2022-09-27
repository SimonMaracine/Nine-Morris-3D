#include <glad/glad.h>

#include "nine_morris_3d_engine/application/capabilities.h"
#include "nine_morris_3d_engine/other/logging.h"

namespace capabilities {
    bool extension_supported(Extension extension) {
        switch (extension) {
            case Extension::AnisotropicFiltering:
                return GLAD_GL_EXT_texture_filter_anisotropic;
            default:
                DEB_CRITICAL("Unknown extension: {}, exiting...", static_cast<int>(extension));
                exit(1);
        }
    }

    int max_samples_supported() {
        GLint max_samples;
        glGetIntegerv(GL_MAX_SAMPLES, &max_samples);

        GLint max_depth_texture_samples;
        glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES, &max_depth_texture_samples);

        GLint max_color_texture_samples;
        glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES, &max_color_texture_samples);

        if (max_depth_texture_samples < max_samples) {
            REL_ERROR("GL_MAX_DEPTH_TEXTURE_SAMPLES < GL_MAX_SAMPLES");
        }

        if (max_color_texture_samples < max_samples) {
            REL_ERROR("GL_MAX_COLOR_TEXTURE_SAMPLES < GL_MAX_SAMPLES");
        }

        return max_samples;
    }
}
