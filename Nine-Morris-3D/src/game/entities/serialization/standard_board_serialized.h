#pragma once

#include "game/entities/serialization/board_serialized.h"

struct StandardBoardSerialized : BoardSerialized {
    std::array<bool, 2> can_jump = { false, false };

    unsigned int white_pieces_count = 0;
    unsigned int black_pieces_count = 0;
    unsigned int not_placed_white_pieces_count = 9;
    unsigned int not_placed_black_pieces_count = 9;

    unsigned int turns_without_mills = 0;
};
