#pragma once

namespace options {
    struct Options {
        int texture_quality = 0;
        int samples = 2;
        bool vsync = true;
    };

    void load_options_from_file(Options& options);
    void save_options_to_file(const Options& options);
}
