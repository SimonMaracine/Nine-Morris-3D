#pragma once

#include <string>

namespace sm {
    namespace openal_debug {
        void check_errors();
        std::string get_information();
        const char* get_version();
    }
}
