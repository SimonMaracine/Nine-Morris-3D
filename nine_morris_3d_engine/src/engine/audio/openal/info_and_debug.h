#pragma once

namespace sm {
    namespace al {
        void maybe_check_errors();

        // Get various information about OpenAL
        std::string get_info();

        const char* get_version();
    }
}
