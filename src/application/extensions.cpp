#include <glad/glad.h>

#include "application/extensions.h"
#include "other/logging.h"

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
