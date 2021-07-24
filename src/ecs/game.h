#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

constexpr float PIECE_Y_POSITION = 0.135f;
constexpr float NODE_Y_POSITION = 0.062f;

constexpr glm::vec3 NODE_POSITIONS[24] = {
    glm::vec3(2.25f, NODE_Y_POSITION, 2.295f),
    glm::vec3(-0.04f, NODE_Y_POSITION, 2.29f),
    glm::vec3(-2.26f, NODE_Y_POSITION, 2.27f),
    glm::vec3(1.6f, NODE_Y_POSITION, 1.65f),
    glm::vec3(-0.04f, NODE_Y_POSITION, 1.65f),
    glm::vec3(-1.66f, NODE_Y_POSITION, 1.65f),
    glm::vec3(0.98f, NODE_Y_POSITION, 1.01f),
    glm::vec3(-0.03f, NODE_Y_POSITION, 1.01f),
    glm::vec3(-0.99f, NODE_Y_POSITION, 1.0f),
    glm::vec3(2.26f, NODE_Y_POSITION, 0.03f),
    glm::vec3(1.61f, NODE_Y_POSITION, 0.03f),
    glm::vec3(0.99f, NODE_Y_POSITION, 0.04f),
    glm::vec3(-0.99f, NODE_Y_POSITION, 0.03f),
    glm::vec3(-1.66f, NODE_Y_POSITION, 0.03f),
    glm::vec3(-2.29f, NODE_Y_POSITION, 0.02f),
    glm::vec3(0.98f, NODE_Y_POSITION, -1.0f),
    glm::vec3(-0.07f, NODE_Y_POSITION, -1.0f),
    glm::vec3(-0.98f, NODE_Y_POSITION, -1.0f),
    glm::vec3(1.6f, NODE_Y_POSITION, -1.62f),
    glm::vec3(-0.06f, NODE_Y_POSITION, -1.63f),
    glm::vec3(-1.65f, NODE_Y_POSITION, -1.63f),
    glm::vec3(2.25f, NODE_Y_POSITION, -2.26f),
    glm::vec3(-0.06f, NODE_Y_POSITION, -2.26f),
    glm::vec3(-2.28f, NODE_Y_POSITION, -2.28f)
};

constexpr int FRAMES = 20;
constexpr float MOVE_SPEED = 0.05f;

enum class Phase {
    PlacePieces,
    MovePieces,
    GameOver
};

enum class Piece {
    White,
    Black,
    None
};

enum class Player {
    White,
    Black
};

struct PieceComponent {
    PieceComponent(Piece type) : type(type) {}

    bool active = false;
    Piece type;
};

struct MoveComponent {
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 target = glm::vec3(0.0f);
    int current_frame = 0;
    bool move = false;
};

struct NodeComponent {
    // Piece type = Piece::None;
    entt::entity piece = entt::null;
};

struct GameStateComponent {
    Phase phase = Phase::PlacePieces;
    Player turn = Player::White;
    int white_pieces_count = 0;  // Number of pieces on the board
    int black_pieces_count = 0;
    entt::entity nodes[24];
};

void game_update_system(entt::registry& registry, entt::entity board, entt::entity hovered);
void piece_move_system(entt::registry& registry, float dt);
