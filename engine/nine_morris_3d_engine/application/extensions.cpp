#include <glad/glad.h>

#include "nine_morris_3d_engine/application/extensions.h"
#include "nine_morris_3d_engine/other/logging.h"

namespace extensions {
    bool extension_supported(Extension extension) {
        switch (extension) {
            case Extension::AnisotropicFiltering:
                return GLAD_GL_EXT_texture_filter_anisotropic;
            default:
                DEB_CRITICAL("Unknown extension: {}, exiting...", static_cast<int>(extension));
                exit(1);
        }
    }
}
