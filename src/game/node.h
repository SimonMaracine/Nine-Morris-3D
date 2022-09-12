#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/constants.h"

struct Piece;

struct Node {
    Node() = default;
    Node(size_t index, hover::Id id)  // FIXME id?
        : index(index) {}

    size_t index = INVALID_NODE_INDEX;  // From 0 through 23

    std::shared_ptr<Renderer::Model> model;

    size_t piece_index = INVALID_PIECE_INDEX;
    Piece* piece = nullptr;  // Reference to the piece that sits on this node
};
