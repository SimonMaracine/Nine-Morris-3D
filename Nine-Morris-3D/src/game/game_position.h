#pragma once

#include "other/constants.h"

struct GamePosition {
    GamePosition();

    PieceType at(size_t index) const;
    PieceType& at(size_t index);

    bool operator==(const GamePosition& other) const;

    std::array<PieceType, MAX_NODES> data;
    unsigned int turns = 0;
};
