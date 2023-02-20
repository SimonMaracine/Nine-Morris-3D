#pragma once

/**
 * Queryable OpenGL implementation and GPU capabilities.
 */
namespace capabilities {
    int max_anisotropic_filtering_supported();
    int max_samples_supported();
    int max_texture_units_supported();
}
