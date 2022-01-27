#pragma once

#include <string>

namespace options {
    static constexpr const char* NORMAL = "normal";
    static constexpr const char* LOW = "low";

    static constexpr const char* FIELD = "field";
    static constexpr const char* AUTUMN = "autumn";

    struct Options {
        std::string texture_quality = NORMAL;  // These are default values
        int samples = 2;
        bool vsync = true;
        bool save_on_exit = true;
        std::string skybox = FIELD;
        bool custom_cursor = true;
    };

    void load_options_from_file(Options& options);
    void save_options_to_file(const Options& options);
    void create_options_file();
}
