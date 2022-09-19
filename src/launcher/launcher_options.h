#pragma once

namespace launcher_options {
    static constexpr const char* LAUNCHER_OPTIONS_FILE = "launcher_options.json";

    static constexpr const char* NORMAL = "normal";
    static constexpr const char* LOW = "low";

    struct LauncherOptions {
        // These are default values
        std::tuple<int, int> resolution = std::make_tuple<int, int>(1024, 576);
        bool fullscreen = false;
        std::string texture_quality = NORMAL;
        bool normal_mapping = true;
        bool bloom = true;
        float bloom_strength = 0.3f;

        template<typename Archive>
        void serialize(Archive& archive) {
            archive(
                resolution,
                fullscreen,
                texture_quality,
                normal_mapping,
                bloom,
                bloom_strength
            );
        }
    };

    std::tuple<bool, std::string> validate(const LauncherOptions& options);
}
