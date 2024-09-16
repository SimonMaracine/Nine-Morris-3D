#pragma once

// Enums and constants that should not change very often!

enum class GameMode {
    Standard,
    Jump,
    JumpPlus
};

enum class GamePlayer {
    Human,
    Computer
};

enum class Skybox {
    None,
    Field,
    Autumn
};

enum class AntiAliasing {
    Off = 1,
    _2x = 2,
    _4x = 4
};

enum class AnisotropicFiltering {
    Off = 0,
    _4x = 4,
    _8x = 8
};

enum class TextureQuality {
    Half,
    Full
};

enum class ShadowQuality {
    Half = 2048,
    Full = 4096
};

enum class Scale {
    _100 = 1,
    _200 = 2,
};

inline constexpr int MIN_WIDTH {512};
inline constexpr int MIN_HEIGHT {288};
inline constexpr int MAX_WIDTH {2560};
inline constexpr int MAX_HEIGHT {1440};

inline constexpr int DEFAULT_WIDTH {1024};
inline constexpr int DEFAULT_HEIGHT {576};
inline constexpr int DEFAULT_WIDTH_LARGE {1600};
inline constexpr int DEFAULT_HEIGHT_LARGE {900};
