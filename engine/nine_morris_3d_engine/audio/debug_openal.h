#pragma once

namespace debug_openal {
    // Get various information about OpenAL
    std::string get_info();

    void check_errors();
    const char* get_version();
}
