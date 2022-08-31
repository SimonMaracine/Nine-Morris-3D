#pragma once

#include <entt/entt.hpp>

#include "game/constants.h"

struct NodeComponent {
    // hover::Id id = hover::null;

    // Renderer::Model model;

    size_t piece_index = INVALID_PIECE_INDEX;
    // Piece* piece = nullptr;  // Reference to the piece that sits on this node
    entt::entity piece = entt::null;  // Handle to the piece that sits on this node

    size_t index = INVALID_NODE_INDEX;  // From 0 to 23, needed for easier coping with these
};
