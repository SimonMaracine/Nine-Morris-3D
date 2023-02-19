#pragma once

#include "game/entities/serialization/board_serialized.h"

struct StandardBoardSerialized : BoardSerialized {
    std::array<bool, 2> can_jump = { false, false };

    unsigned int turns_without_mills = 0;
};
