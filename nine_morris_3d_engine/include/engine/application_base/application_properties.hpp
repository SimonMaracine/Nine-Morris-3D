#pragma once

#include <string>

namespace sm {
    /*
        Struct for constant application properties
    */
    struct ApplicationProperties {
        bool fullscreen {};
        bool native_resolution {};
        bool resizable {};
        int width {};  // Initial width and height
        int height {};
        int min_width {};
        int min_height {};
        unsigned int version_major {};
        unsigned int version_minor {};
        unsigned int version_patch {};
        std::string title;
        std::string app_name;
    };
}
