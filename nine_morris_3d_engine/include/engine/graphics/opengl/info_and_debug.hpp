#pragma once

#include <string>
#include <utility>

namespace sm {
    struct GlInfoDebug {
        static void maybe_initialize_debugging();

        // Get various information about OpenGL
        static std::string get_info();

        static std::pair<int, int> get_version_number();
        static const unsigned char* get_opengl_version();
        static const unsigned char* get_glsl_version();
        static const unsigned char* get_vendor();
        static const unsigned char* get_renderer();
    };
}
