#pragma once

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#define PIECE(entity) registry.get<PieceComponent>(entity)
#define NODE(entity) registry.get<NodeComponent>(entity)
#define STATE(entity) registry.get<GameStateComponent>(entity)

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

constexpr float PIECE_MOVE_SPEED = 1.5f;

enum class Phase {
    PlacePieces,
    MovePieces,
    GameOver,
    None
};

enum class Piece {
    White,
    Black,
};

enum class Player {
    White = 0,
    Black = 1
};

enum class Ending {
    None,
    WinnerWhite,
    WinnerBlack,
    TieBetweenBothPlayers
};

struct PieceComponent {
    PieceComponent(Piece type) : type(type) {}

    Piece type;
    bool active = false;
    entt::entity node = entt::null;

    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
    bool selected = false;
};

struct MoveComponent {
    glm::vec3 velocity = glm::vec3(0.0f);
    glm::vec3 target = glm::vec3(0.0f);
    bool should_move = false;

    float distance_travelled = 0.0f;
    glm::vec3 distance_to_travel = glm::vec3(0.0f);
};

struct NodeComponent {
    NodeComponent(int id) : id(id) {}

    int id;
    entt::entity piece = entt::null;
};

struct GameStateComponent {
    GameStateComponent(entt::entity* nodes) {
        for (int i = 0; i < 24; i++) {
            this->nodes[i] = nodes[i];
        }
    }

    Phase phase = Phase::PlacePieces;
    Player turn = Player::White;
    Ending ending = Ending::None;

    int white_pieces_count = 0;  // Number of pieces on the board
    int black_pieces_count = 0;
    int not_placed_pieces_count = 18;
    bool should_take_piece = false;

    entt::entity nodes[24];

    entt::entity pressed_node = entt::null;
    entt::entity pressed_piece = entt::null;

    entt::entity selected_piece = entt::null;

    bool can_jump[2] = { false, false };
};

namespace systems {
    void place_piece(entt::registry& registry, entt::entity board, entt::entity hovered);
    void move_pieces(entt::registry& registry, float dt);
    void take_piece(entt::registry& registry, entt::entity board, entt::entity hovered);
    void select_piece(entt::registry& registry, entt::entity board, entt::entity hovered);
    void put_piece(entt::registry& registry, entt::entity board, entt::entity hovered);
    void press(entt::registry& registry, entt::entity board, entt::entity hovered);
    void release(entt::registry& registry, entt::entity board);
}
