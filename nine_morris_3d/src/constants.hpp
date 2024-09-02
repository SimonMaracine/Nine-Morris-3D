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

enum Skybox {
    None,
    Field,
    Autumn
};

inline constexpr int MIN_WIDTH {512};
inline constexpr int MIN_HEIGHT {288};
inline constexpr int MAX_WIDTH {2560};
inline constexpr int MAX_HEIGHT {1440};

inline constexpr int DEFAULT_WIDTH {1024};
inline constexpr int DEFAULT_HEIGHT {576};
inline constexpr int DEFAULT_WIDTH_LARGE {1600};
inline constexpr int DEFAULT_HEIGHT_LARGE {900};
