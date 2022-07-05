#pragma once

#include <glm/glm.hpp>

#include "graphics/renderer/renderer.h"
#include "graphics/renderer/hoverable.h"
#include "nine_morris_3d/constants.h"

struct Node;

struct Piece {
    enum Type {
        White = 0,
        Black = 1,
        None
    };

    enum class MovementType {
        None,
        Linear,
        ThreeStep
    };

    Piece() = default;
    Piece(hoverable::Id id, Type type, size_t index)
        : id(id), type(type), index(index) {}

    hoverable::Id id = hoverable::null;

    Renderer::Model model;

    struct Movement {
        MovementType type = MovementType::None;
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 target = glm::vec3(0.0f);
        glm::vec3 target0 = glm::vec3(0.0f);
        glm::vec3 target1 = glm::vec3(0.0f);
        bool reached_target0 = false;
        bool reached_target1 = false;
    } movement;

    bool should_move = false;

    Type type = Type::None;
    bool in_use = false;  // true, if it is on the board
    size_t node_index = INVALID_NODE_INDEX;
    Node* node = nullptr;  // Reference to the node where it sits on

    size_t index = INVALID_PIECE_INDEX;  // From 0 to 17, needed for easier coping with these

    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
    bool selected = false;

    bool active = true;  // Not active pieces simply don't exist in the game, only in memory

    bool renderer_with_outline = false;
};
