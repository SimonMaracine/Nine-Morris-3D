#pragma once

#include "game/piece_movement.h"
#include "other/constants.h"

struct PieceSerialized {
    Index index = NULL_INDEX;

    PieceType type = PieceType::None;
    bool in_use = false;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);

    Index node_index = NULL_INDEX;

    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
};
