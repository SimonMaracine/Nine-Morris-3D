#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

using namespace sm::localization_literals;

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
        return "white"_L;
    } else {
        return "black"_L;
    }
}
