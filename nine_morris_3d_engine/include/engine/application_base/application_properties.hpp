#pragma once

#include <string>

namespace sm {
    /*
        Struct for constant application properties
    */
    struct ApplicationProperties {
        int width {};  // Initial width and height
        int height {};
        std::string title;
        bool fullscreen {};
        bool native_resolution {};
        bool resizable {};
        int min_width {};
        int min_height {};
        std::string app_name;
        unsigned int version_major {};
        unsigned int version_minor {};
        unsigned int version_patch {};
    };
}
