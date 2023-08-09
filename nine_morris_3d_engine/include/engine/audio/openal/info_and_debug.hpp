#pragma once

#include <string>

namespace sm {
    struct AlInfoDebug {
        static void maybe_check_errors();

        // Get various information about OpenAL
        static std::string get_info();

        static const char* get_version();
    };
}
