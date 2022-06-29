#pragma once

#include "graphics/renderer/renderer.h"
#include "nine_morris_3d/board.h"

class KeyboardControls {
public:
    KeyboardControls(Board& board);
    ~KeyboardControls() = default;

    enum class Direction {
        Up = 0,
        Left = 1,
        Down = 2,
        Right = 3
    };

    void render();
    void move(Direction direction);
    void press();

    static Direction calculate(Direction original_direction, float camera_angle); 

    Renderer::Quad quad;
private:
    struct Node {
        Node() = default;
        Node(size_t index)
            : index(index) {}

        void neighbors(Node* up, Node* down, Node* left, Node* right) {
            neighbor_up = up;
            neighbor_down = down;
            neighbor_left = left;
            neighbor_right = right;
        }

        size_t index;
        Node* neighbor_up = nullptr;
        Node* neighbor_down = nullptr;
        Node* neighbor_left = nullptr;
        Node* neighbor_right = nullptr;
    };

    Node nodes[24];
    Node* current_node = nullptr;
    Board& board;
};
