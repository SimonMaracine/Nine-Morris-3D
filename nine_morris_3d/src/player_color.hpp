#pragma once

// Generic player type
enum PlayerColor : int {
    PlayerColorWhite,
    PlayerColorBlack
};

inline PlayerColor opponent(PlayerColor color) {
    if (color == PlayerColorWhite) {
        return PlayerColorBlack;
    } else {
        return PlayerColorWhite;
    }
}

inline const char* to_string(PlayerColor color) {
    if (color == PlayerColorWhite) {
        return "white";
    } else {
        return "black";
    }
}
