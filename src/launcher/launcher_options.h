#pragma once

namespace launcher_options {
    static constexpr const char* LAUNCHER_OPTIONS_FILE = "launcher_options.json";

    static constexpr const char* NORMAL = "normal";
    static constexpr const char* LOW = "low";

    struct LauncherOptions {
        // These are default values
        bool fullscreen = false;
        bool native_resolution = true;
        std::pair<int, int> resolution = {1024, 576};
        int samples = 2;
        int anisotropic_filtering = 4;
        std::string texture_quality = NORMAL;
        bool normal_mapping = true;
        bool bloom = true;
        float bloom_strength = 0.3f;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(
                fullscreen,
                native_resolution,
                resolution,
                samples,
                anisotropic_filtering,
                texture_quality,
                normal_mapping,
                bloom,
                bloom_strength
            );
        }
    };

    std::pair<bool, std::string> validate(const LauncherOptions& options);
}
