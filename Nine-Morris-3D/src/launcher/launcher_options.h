#pragma once

#include "other/constants.h"

namespace launcher_options {
    inline constexpr const char* LAUNCHER_OPTIONS_FILE = "launcher_options.dat";

    inline constexpr int NORMAL = static_cast<int>(TextureQuality::Normal);
    inline constexpr int LOW = static_cast<int>(TextureQuality::Low);

    inline constexpr bool DEFAULT_FULLSCREEN = false;
    inline constexpr bool DEFAULT_NATIVE_RESOLUTION = true;
    inline constexpr std::pair<int, int> DEFAULT_RESOLUTION = {1024, 576};
    inline constexpr int DEFAULT_SAMPLES = 2;
    inline constexpr int DEFAULT_ANISOTROPIC_FILTERING = 4;
    inline constexpr int DEFAULT_TEXTURE_QUALITY = NORMAL;
    inline constexpr bool DEFAULT_NORMAL_MAPPING = true;
    inline constexpr bool DEFAULT_BLOOM = true;
    inline constexpr float DEFAULT_BLOOM_STRENGTH = 0.3f;

    struct LauncherOptions {
        bool fullscreen = DEFAULT_FULLSCREEN;
        bool native_resolution = DEFAULT_NATIVE_RESOLUTION;
        std::pair<int, int> resolution = DEFAULT_RESOLUTION;
        int samples = DEFAULT_SAMPLES;
        int anisotropic_filtering = DEFAULT_ANISOTROPIC_FILTERING;
        int texture_quality = DEFAULT_TEXTURE_QUALITY;
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

    std::optional<std::string> validate_load(const LauncherOptions& options);
}
