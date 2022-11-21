#pragma once

#include "nine_morris_3d_engine/application/platform.h"
#include "nine_morris_3d_engine/other/logging.h"

namespace debug_opengl {
    void maybe_initialize_debugging();

    // Get information about OpenGL and dependencies
    std::string get_info();

    std::pair<int, int> get_version_numbers();
    const unsigned char* get_opengl_version();
    const unsigned char* get_glsl_version();
    const unsigned char* get_vendor();
    const unsigned char* get_renderer();
}
