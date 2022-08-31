#pragma once

namespace extensions {
    enum Extension {
        AnisotropicFiltering
    };

    bool extension_supported(Extension extension);
}
