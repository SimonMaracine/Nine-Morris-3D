#pragma once

#include <engine/engine_scene.h>

#include "other/constants.h"

struct Node {
    Node() = default;
    Node(Index index, renderables::Model* model)
        : index(index), model(model) {}
    ~Node() = default;

    Node(const Node&) = delete;
    Node& operator=(const Node&) = delete;
    Node(Node&&) noexcept = default;
    Node& operator=(Node&&) noexcept = default;

    Index index = NULL_INDEX;  // From 0 through 23

    renderables::Model* model = nullptr;

    Index piece_index = NULL_INDEX;  // Reference to the piece that sits on this node
};
