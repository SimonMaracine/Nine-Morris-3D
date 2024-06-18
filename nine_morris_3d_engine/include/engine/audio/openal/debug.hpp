#pragma once

#include <string>

namespace sm {
    struct AlDebug {
        static void check_errors();
        static std::string get_information();
        static const char* get_version();
    };
}
