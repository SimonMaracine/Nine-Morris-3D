#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/constants.h"

struct Piece;

struct Node {
    Node() = default;
    Node(size_t index, std::shared_ptr<Renderer::Model> model)  // FIXME id?
        : index(index), model(model) {}

    size_t index = NULL_INDEX;  // From 0 through 23

    std::shared_ptr<Renderer::Model> model;

    size_t piece = NULL_INDEX;  // Reference to the piece that sits on this node
};
