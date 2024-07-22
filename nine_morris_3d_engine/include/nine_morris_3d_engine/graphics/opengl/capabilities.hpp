#pragma once

// Query OpenGL implementation and GPU capabilities
namespace sm {
    namespace capabilities {
        int max_anisotropic_filtering_supported();
        int max_samples_supported();
        int max_texture_units_supported();
        bool is_srgb_capable();
    }
}
