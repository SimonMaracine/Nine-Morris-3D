#pragma once

#include <string>

namespace options {
    static constexpr const char* NORMAL = "normal";
    static constexpr const char* LOW = "low";

    static constexpr const char* FIELD = "field";
    static constexpr const char* AUTUMN = "autumn";

    struct Options {
        // These are default values
        std::string texture_quality = NORMAL;
        int samples = 2;
        bool vsync = true;
        bool save_on_exit = true;
        std::string skybox = FIELD;
        bool custom_cursor = true;
        float sensitivity = 1.0f;
    };

    void load_options_from_file(Options& options);
    void save_options_to_file(const Options& options);
    void create_options_file();
}
