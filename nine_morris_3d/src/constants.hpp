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

inline constexpr int MAX_WIDTH {2560};
inline constexpr int MAX_HEIGHT {1440};
inline constexpr int MIN_WIDTH {512};
inline constexpr int MIN_HEIGHT {288};
