#pragma once

#include <string>

namespace sm {
    // Constant application properties
    struct ApplicationProperties {
        bool fullscreen {false};
        bool native_resolution {true};
        bool resizable {true};
        bool audio {false};
        int width {1280};  // Initial width and height
        int height {720};
        int min_width {-1};
        int min_height {-1};
        unsigned int version_major {0};
        unsigned int version_minor {1};
        unsigned int version_patch {0};
        std::string title {"Nine Morris 3D Engine"};
        std::string app_name {"NineMorris3DEngine"};
        void* user_data {nullptr};
    };
}
