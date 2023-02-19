#pragma once

#include "other/constants.h"

struct GamePosition {
    GamePosition();
    ~GamePosition() = default;

    GamePosition(const GamePosition&) = default;
    GamePosition& operator=(const GamePosition&) = default;
    GamePosition(GamePosition&&) noexcept = default;
    GamePosition& operator=(GamePosition&&) noexcept = default;

    PieceType at(size_t index) const;
    PieceType& at(size_t index);

    bool operator==(const GamePosition& other) const;

    std::array<PieceType, MAX_NODES> data;
    unsigned int turns = 0;
    unsigned int white_pieces_on_board = 0;
    unsigned int black_pieces_on_board = 0;
    unsigned int white_pieces_outside = 0;
    unsigned int black_pieces_outside = 0;
};
