#pragma once

#include <glm/glm.hpp>

#include "opengl/renderer/vertex_array.h"
#include "opengl/renderer/texture.h"
#include "nine_morris_3d/node.h"
#include "nine_morris_3d/hoverable.h"

constexpr float PIECE_Y_POSITION = 0.135f;
constexpr float PIECE_MOVE_SPEED = 1.5f;

class Piece : public Hoverable {
public:
    enum class Type {
        White,
        Black
    };

    Piece(unsigned int id, Type type);
    ~Piece() = default;

    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    static float scale;

    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 target = glm::vec3(0.0f);
    bool should_move = false;
    float distance_travelled = 0.0f;
    glm::vec3 distance_to_travel = glm::vec3(0.0f);

    Rc<VertexArray> vertex_array;
    static int index_count;
    Rc<Texture> diffuse_texture;

    static glm::vec3 specular_color;
    static float shininess;

    static glm::vec3 select_color;
    static glm::vec3 hover_color;

    Type type;
    bool in_use = false;
    Node* node = nullptr;  // Reference to the node where it sits on

    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
    bool selected = false;

    // int id;  // Hmmm
};
