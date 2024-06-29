#pragma once

#include <string>
#include <utility>

namespace sm {
    namespace opengl_debug {
        void initialize();
        std::string get_information();
        std::pair<int, int> get_version_number();
        const unsigned char* get_opengl_version();
        const unsigned char* get_glsl_version();
        const unsigned char* get_vendor();
        const unsigned char* get_renderer();
    }
}
