#pragma once

// Query OpenGL implementation and GPU capabilities
namespace sm {
    namespace capabilities {
        int max_anisotropic_filtering_supported() noexcept;
        int max_samples_supported() noexcept;
        int max_texture_units_supported() noexcept;
        bool is_srgb_capable() noexcept;
    }
}
