#pragma once

#include <glm/glm.hpp>

#include "graphics/renderer/renderer.h"
#include "graphics/renderer/hoverable.h"
#include "nine_morris_3d/piece.h"
#include "nine_morris_3d/constants.h"

struct Node {
    Node() = default;
    Node(hoverable::Id id, size_t index)
        : id(id), index(index) {}

    hoverable::Id id = hoverable::null;

    Renderer::Model model;

    size_t piece_index = INVALID_PIECE_INDEX;
    Piece* piece = nullptr;  // Reference to the piece that sits on this node

    size_t index = INVALID_NODE_INDEX;  // From 0 to 23, needed for easier coping with these
};
