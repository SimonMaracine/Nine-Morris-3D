#pragma once

#include <entt/entt.hpp>

struct KeyboardControlsComponent {
    struct KNode {
        KNode() = default;
        KNode(size_t index)
            : index(index) {}

        void neighbors(KNode* up, KNode* down, KNode* left, KNode* right) {
            neighbor_up = up;
            neighbor_down = down;
            neighbor_left = left;
            neighbor_right = right;
        }

        size_t index = 0;
        KNode* neighbor_up = nullptr;
        KNode* neighbor_down = nullptr;
        KNode* neighbor_left = nullptr;
        KNode* neighbor_right = nullptr;
    };

    std::array<KNode, 24> nodes;
    KNode* current_node = nullptr;
};
