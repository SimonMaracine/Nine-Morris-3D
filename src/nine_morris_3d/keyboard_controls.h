#pragma once

#include "graphics/renderer/renderer.h"
#include "nine_morris_3d/board.h"

class KeyboardControls {
public:
    KeyboardControls() = default;
    KeyboardControls(Board* board);
    ~KeyboardControls() = default;

    enum class Direction {
        Up = 0,
        Left = 1,
        Down = 2,
        Right = 3
    };

    void initialize();
    void move(Direction direction);
    bool press(bool& first_move);

    static Direction calculate(Direction original_direction, float camera_angle); 

    Renderer::Quad quad;
private:
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

    KNode nodes[24];
    KNode* current_node = nullptr;
    Board* board = nullptr;
};
