#pragma once

namespace capabilities {
    enum Extension {
        AnisotropicFiltering
    };

    bool extension_supported(Extension extension);
    int max_samples_supported();
}
