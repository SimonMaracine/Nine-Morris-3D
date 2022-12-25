#pragma once

#include "engine/application/platform.h"
#include "engine/other/logging.h"

namespace gl {
    void maybe_initialize_debugging();

    // Get various information about OpenGL
    std::string get_info();

    std::pair<int, int> get_version_number();
    const unsigned char* get_opengl_version();
    const unsigned char* get_glsl_version();
    const unsigned char* get_vendor();
    const unsigned char* get_renderer();
}
