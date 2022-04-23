#pragma once

#include <glm/glm.hpp>

#include "graphics/renderer/main_renderer.h"
#include "graphics/renderer/hoverable.h"

class Node;

constexpr float PIECE_Y_POSITION = 0.135f;
constexpr float PIECE_BASE_VELOCITY = 0.3f;
constexpr float PIECE_VARIABLE_VELOCITY = 10.0f;
constexpr float PIECE_THREESTEP_HEIGHT = 0.4f;
constexpr float PIECE_RAISE_HEIGHT = 1.3f;

class Piece {
public:
    enum Type {
        White,
        Black,
        None
    };

    enum class MovementType {
        None,
        Linear,
        ThreeStep
    };

    Piece() = default;
    Piece(hoverable::Id id, Type type)
        : id(id), type(type) {}
    ~Piece() = default;

    hoverable::Id id = hoverable::null;

    Renderer::Model model;

    struct Movement {
        MovementType type = MovementType::None;
        glm::vec3 velocity = glm::vec3(0.0f);
        glm::vec3 target = glm::vec3(0.0f);
        glm::vec3 target0 = glm::vec3(0.0f);
        glm::vec3 target1 = glm::vec3(0.0f);
        bool reached_target0 = false;
        bool reached_target1 = false;
    } movement;

    bool should_move = false;

    Type type = Type::None;
    bool in_use = false;  // true, if it is on the board
    hoverable::Id node_id = hoverable::null;
    Node* node = nullptr;  // Reference to the node where it sits on

    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
    bool selected = false;

    bool active = true;  // Not active pieces simply don't exist in the game, only in memory

    bool renderer_with_outline = false;
};
