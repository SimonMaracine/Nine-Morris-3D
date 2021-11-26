#pragma once

#include <glm/glm.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/texture.h"
#include "nine_morris_3d/hoverable.h"

class Node;

constexpr float PIECE_Y_POSITION = 0.135f;
constexpr float PIECE_MOVE_SPEED = 2.6f;

class Piece {
public:
    enum Type {
        White,
        Black,
        None
    };

    Piece() = default;
    Piece(hoverable::Id id, Type type);
    ~Piece() = default;

    hoverable::Id id;

    glm::vec3 position;
    glm::vec3 rotation;
    float scale;

    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 target = glm::vec3(0.0f);
    bool should_move = false;
    float distance_travelled = 0.0f;
    glm::vec3 distance_to_travel = glm::vec3(0.0f);

    Rc<VertexArray> vertex_array;
    int index_count;
    Rc<Texture> diffuse_texture;

    glm::vec3 specular_color;
    float shininess;

    glm::vec3 select_color;
    glm::vec3 hover_color;

    Type type;
    bool in_use = false;
    Node* node = nullptr;  // Reference to the node where it sits on
    hoverable::Id node_id = hoverable::null;

    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
    bool selected = false;

    bool active = true;  // Not active pieces simply don't exist in the game, only in memory

    // int id;  // Hmmm
};
