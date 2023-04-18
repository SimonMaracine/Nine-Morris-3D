#pragma once

#include <engine/public/scene.h>

#include "other/constants.h"

struct Node {
    Node() = default;
    Node(size_t index, sm::renderables::Model* model)
        : index(index), model(model) {}
    ~Node() = default;

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;

    size_t index = NULL_INDEX;  // From 0 through 23

    sm::renderables::Model* model = nullptr;

    size_t piece_index = NULL_INDEX;  // Reference to the piece that sits on this node
};
