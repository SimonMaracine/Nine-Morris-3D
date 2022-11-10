#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "other/constants.h"

struct Node {
    Node() = default;
    Node(size_t index, std::shared_ptr<Renderer::Model> model)
        : index(index), model(model) {}
    ~Node() = default;

    Node(const Node&) = delete;
    Node(Node&&) = default;
    Node& operator=(const Node&) = delete;
    Node& operator=(Node&&) = default;

    size_t index = NULL_INDEX;  // From 0 through 23

    std::shared_ptr<Renderer::Model> model;

    size_t piece_index = NULL_INDEX;  // Reference to the piece that sits on this node
};
