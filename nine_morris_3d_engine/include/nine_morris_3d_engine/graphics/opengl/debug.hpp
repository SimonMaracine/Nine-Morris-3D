#pragma once

#include <string>
#include <utility>

// Retrieve information about the OpenGL context

namespace sm {
    namespace opengl_debug {
        void initialize();
        std::string get_information();
        std::pair<int, int> get_version_number();
        const char* get_opengl_version();
        const char* get_glsl_version();
        const char* get_renderer();
        const char* get_vendor();
    }
}
