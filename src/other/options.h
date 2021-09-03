#pragma once

#include <string>

namespace options {
    struct Options {
        int texture_quality = 0;
        int samples = 2;
        bool vsync = true;
    };

    void get_options_from_file(Options& options);
}
