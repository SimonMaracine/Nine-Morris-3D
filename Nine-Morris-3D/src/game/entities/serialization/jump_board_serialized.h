#pragma once

#include "game/entities/serialization/board_serialized.h"

struct JumpBoardSerialized : BoardSerialized {
    unsigned int turns_without_mills = 0;
};
