#pragma once

// Enums and constants that should not change very often!

enum class GameMode {
    Standard,
    Jump,
    JumpPlus
};

enum class PlayerType {
    Human,
    Computer
};

enum class Skybox {
    None,
    Field,
    Autumn
};

enum class AntiAliasing {
    Off,
    _2x,
    _4x
};

enum class AnisotropicFiltering {
    Off,
    _4x,
    _8x
};

enum class TextureQuality {
    Half,
    Full
};

inline constexpr int MIN_WIDTH {512};
inline constexpr int MIN_HEIGHT {288};
inline constexpr int MAX_WIDTH {2560};
inline constexpr int MAX_HEIGHT {1440};

inline constexpr int DEFAULT_WIDTH {1024};
inline constexpr int DEFAULT_HEIGHT {576};
inline constexpr int DEFAULT_WIDTH_LARGE {1600};
inline constexpr int DEFAULT_HEIGHT_LARGE {900};
