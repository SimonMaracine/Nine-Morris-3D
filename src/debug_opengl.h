#pragma once

#include <string>
#include <utility>

namespace debug_opengl {
    const std::string get_info();
    const std::pair<int, int> get_opengl_version();
}
