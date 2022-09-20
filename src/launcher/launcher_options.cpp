#include "launcher/launcher_options.h"

namespace launcher_options {
    std::pair<bool, std::string> validate(const LauncherOptions& options) {
        if (options.texture_quality != NORMAL && options.texture_quality != LOW) {
            return {false, "Options file is invalid: texture_quality"};
        }

        if (options.bloom_strength < 0.1f || options.bloom_strength > 1.0f) {
            return {false, "Options file is invalid: bloom_strength"};
        }

        return {true, ""};
    }
}
