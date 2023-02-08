#pragma once

#include <engine/engine_application.h>
#include <engine/engine_scene.h>

#include "game/entities/board.h"

class Application;

class KeyboardControls {
public:
    KeyboardControls() = default;
    KeyboardControls(Application* app, Board* board, object::Quad* quad);
    ~KeyboardControls() = default;

    KeyboardControls(const KeyboardControls&) = delete;
    KeyboardControls& operator=(const KeyboardControls&) = delete;
    KeyboardControls(KeyboardControls&&) = default;
    KeyboardControls& operator=(KeyboardControls&&) = default;

    enum class Direction {
        Up = 0,
        Left = 1,
        Down = 2,
        Right = 3
    };

    void post_initialize();
    void move(Direction direction);
    Board::Flags click_and_release();

    static Direction calculate(Direction original_direction, float camera_angle);
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

    Board* board = nullptr;

    object::Quad* quad = nullptr;
    KNode nodes[24];
    KNode* current_node = nullptr;
};
