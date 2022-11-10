#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/piece_movement.h"
#include "other/constants.h"

struct PieceSerialized {
    PieceSerialized() = default;
    ~PieceSerialized() = default;

    PieceSerialized(const PieceSerialized&) = default;
    PieceSerialized(PieceSerialized&&) = delete;
    PieceSerialized& operator=(const PieceSerialized&) = default;
    PieceSerialized& operator=(PieceSerialized&&) = default;

    size_t index = NULL_INDEX;

    PieceType type = PieceType::None;
    bool in_use = false;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);

    size_t node_index = NULL_INDEX;

    PieceMovement movement;

    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
    bool selected = false;
};
