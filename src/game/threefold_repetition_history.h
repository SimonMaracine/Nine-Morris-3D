#pragma once

#include "other/constants.h"

struct ThreefoldRepetitionHistory {
    struct PositionPlusInfo {
        bool operator==(const PositionPlusInfo& other) const {
            return (
                position == other.position &&
                piece_index == other.piece_index &&
                node_index == other.node_index
            );
        }

        std::array<PieceType, 24> position;
        size_t piece_index;
        size_t node_index;
    };

    std::vector<PositionPlusInfo> ones;
    std::vector<PositionPlusInfo> twos;
};
