#pragma once

#include <filesystem>

namespace sm {
    // Initialization application properties
    struct ApplicationProperties {
        bool fullscreen {false};
        bool native_resolution {true};
        bool resizable {true};
        bool audio {false};
        bool default_renderer_parameters {true};
        int width {1280};
        int height {720};
        int min_width {-1};
        int min_height {-1};
        unsigned int version_major {0};
        unsigned int version_minor {1};
        unsigned int version_patch {0};
        const char* title {"Nine Morris 3D Engine"};
        const char* log_file {"nine_morris_3d_engine.log"};
        const char* assets_directory {"assets"};
        const char* build_date {"unknown"};
        const char* build_time {"unknown"};
        std::filesystem::path path_logs;
        std::filesystem::path path_saved_data;
        std::filesystem::path path_assets;
        void* user_data {};
    };
}
