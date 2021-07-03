#pragma once

#include <string>
#include <utility>

namespace debug_opengl {
    void maybe_init_debugging();
    const std::string get_info();
    const std::pair<int, int> get_version();
}
