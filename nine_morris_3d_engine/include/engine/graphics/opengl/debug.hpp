#pragma once

#include <string>
#include <utility>

namespace sm {
    struct GlDebug {
        static void initialize_debug();
        static std::string get_information();
        static std::pair<int, int> get_version_number();
        static const unsigned char* get_opengl_version();
        static const unsigned char* get_glsl_version();
        static const unsigned char* get_vendor();
        static const unsigned char* get_renderer();
    };
}
