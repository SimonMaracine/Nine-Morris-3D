#include "launcher/launcher_options.h"

namespace launcher_options {
    std::optional<std::string> validate_load(const LauncherOptions& options) {
        if (options.samples != 1 && options.samples != 2 && options.samples != 4) {
            return std::make_optional("Options file is invalid: samples");
        }

        if (options.anisotropic_filtering != 0 && options.anisotropic_filtering != 4
                && options.anisotropic_filtering != 8) {
            return std::make_optional("Options file is invalid: anisotropic_filtering");
        }

        if (options.texture_quality != NORMAL && options.texture_quality != LOW) {
            return std::make_optional("Options file is invalid: texture_quality");
        }

        if (options.bloom_strength < 0.1f || options.bloom_strength > 1.0f) {
            return std::make_optional("Options file is invalid: bloom_strength");
        }

        return std::nullopt;
    }
}
