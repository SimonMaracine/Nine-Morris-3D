#pragma once

/*
    Query OpenGL implementation and GPU capabilities
*/
namespace sm {
    struct Capabilities {
        static int max_anisotropic_filtering_supported();
        static int max_samples_supported();
        static int max_texture_units_supported();
    };
}
