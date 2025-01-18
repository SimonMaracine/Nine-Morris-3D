#pragma once

// Enums and constants that should not change very often!

enum class NineMensMorrisTime {
    _1min,
    _3min,
    _10min,
    _60min,
    Custom
};

enum class GameType {
    LocalHumanVsHuman,
    LocalHumanVsComputer,
    Online
};

enum class GameMode {
    NineMensMorris
};

enum class GamePlayer {
    Human,
    Computer,
    Remote
};

// Generic type
enum class PlayerColor {
    White,
    Black
};

constexpr const char* player_color_to_string(PlayerColor color) {
    switch (color) {
        case PlayerColor::White:
            return "white";
        case PlayerColor::Black:
            return "black";
    }

    return {};
}

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
    _200 = 2
};

inline constexpr int MIN_WIDTH {512};
inline constexpr int MIN_HEIGHT {288};
inline constexpr int MAX_WIDTH {2560};
inline constexpr int MAX_HEIGHT {1440};

inline constexpr int DEFAULT_WIDTH {1024};
inline constexpr int DEFAULT_HEIGHT {576};
inline constexpr int DEFAULT_WIDTH_LARGE {1600};
inline constexpr int DEFAULT_HEIGHT_LARGE {900};
