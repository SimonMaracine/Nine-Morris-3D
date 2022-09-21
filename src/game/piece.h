#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/constants.h"

struct Node;

struct Piece {
    Piece() = default;
    Piece(size_t index, PieceType type, hover::Id id)  // FIXME id?
        : index(index), type(type) {}

    size_t index = INVALID_INDEX;  // From 0 through 17

    PieceType type = PieceType::None;
    bool in_use = false;  // true, if it is on the board

    std::shared_ptr<Renderer::Model> model;

    size_t node_index = INVALID_INDEX;
    Node* node = nullptr;  // Reference to the node where it sits on

    struct Movement {
        PieceMovementType type = PieceMovementType::None;
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 target = glm::vec3(0.0f);
        glm::vec3 target0 = glm::vec3(0.0f);
        glm::vec3 target1 = glm::vec3(0.0f);
        bool reached_target0 = false;
        bool reached_target1 = false;
        bool moving = false;
    } movement;
    
    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
    bool selected = false;

    bool active = true;  // Not active pieces simply don't exist in the game, only in memory
};
