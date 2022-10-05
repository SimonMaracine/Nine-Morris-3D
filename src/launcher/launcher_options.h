#pragma once

namespace launcher_options {
    static constexpr const char* LAUNCHER_OPTIONS_FILE = "launcher_options.dat";

    static constexpr const char* NORMAL = "normal";
    static constexpr const char* LOW = "low";

    static constexpr bool DEFAULT_FULLSCREEN = false;
    static constexpr bool DEFAULT_NATIVE_RESOLUTION = true;
    static constexpr std::pair<int, int> DEFAULT_RESOLUTION = {1024, 576};
    static constexpr int DEFAULT_SAMPLES = 1;
    static constexpr int DEFAULT_ANISOTROPIC_FILTERING = 4;
    static constexpr const char* DEFAULT_TEXTURE_QUALITY = NORMAL;
    static constexpr bool DEFAULT_NORMAL_MAPPING = true;
    static constexpr bool DEFAULT_BLOOM = true;
    static constexpr float DEFAULT_BLOOM_STRENGTH = 0.3f;

    struct LauncherOptions {
        bool fullscreen = DEFAULT_FULLSCREEN;
        bool native_resolution = DEFAULT_NATIVE_RESOLUTION;
        std::pair<int, int> resolution = DEFAULT_RESOLUTION;
        int samples = DEFAULT_SAMPLES;
        int anisotropic_filtering = DEFAULT_ANISOTROPIC_FILTERING;
        std::string texture_quality = DEFAULT_TEXTURE_QUALITY;
        bool normal_mapping = DEFAULT_NORMAL_MAPPING;
        bool bloom = DEFAULT_BLOOM;
        float bloom_strength = DEFAULT_BLOOM_STRENGTH;

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
