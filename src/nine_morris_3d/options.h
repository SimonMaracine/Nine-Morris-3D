#pragma once

#include <string>

namespace options {
    struct Options {
        std::string texture_quality = "normal";
        int samples = 2;
        bool vsync = true;
        bool save_on_exit = true;
    };

    void load_options_from_file(Options& options);
    void save_options_to_file(const Options& options);
    void create_options_file();
}
