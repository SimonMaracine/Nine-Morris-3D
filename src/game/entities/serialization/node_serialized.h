#pragma once

#include "other/constants.h"

struct NodeSerialized {
    NodeSerialized() = default;
    ~NodeSerialized() = default;

    NodeSerialized(const NodeSerialized&) = default;
    NodeSerialized(NodeSerialized&&) = delete;
    NodeSerialized& operator=(const NodeSerialized&) = default;
    NodeSerialized& operator=(NodeSerialized&&) = default;

    size_t index = NULL_INDEX;
    size_t piece_index = NULL_INDEX;
};
