#pragma once

namespace sm {
    // Initialization application properties
    struct ApplicationProperties {
        bool fullscreen {false};
        bool native_resolution {true};
        bool resizable {true};
        bool audio {false};
        int width {1280};
        int height {720};
        int min_width {-1};
        int min_height {-1};
        int samples {1};
        unsigned int version_major {0};
        unsigned int version_minor {1};
        unsigned int version_patch {0};
        const char* title {"Nine Morris 3D Engine"};
        const char* application_name {"NineMorris3DEngine"};
        const char* info_file {};
        const char* log_file {};
        const char* assets_directory {};
        void* user_data {};
    };
}
