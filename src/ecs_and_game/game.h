#pragma once

#include <vector>
#include <unordered_map>

#include <entt/entt.hpp>
#include <glm/glm.hpp>

#include "ecs_and_game/components.h"
#include "opengl/renderer/renderer.h"
#include "other/loader.h"

#define PIECE(entity) registry.get<PieceComponent>(entity)
#define NODE(entity) registry.get<NodeComponent>(entity)
#define STATE(entity) registry.get<GameStateComponent>(entity)
#define MOVES_HISTORY(entity) registry.get<MovesHistoryComponent>(entity)
#define TRANSFORM(entity) registry.get<TransformComponent>(entity)

constexpr float PIECE_Y_POSITION = 0.135f;
constexpr float NODE_Y_POSITION = 0.063f;

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

constexpr glm::vec3 AFTER_UNDO_POSITION = glm::vec3(0.0f, 0.45f, 0.0f);

constexpr float PIECE_MOVE_SPEED = 1.5f;
constexpr int MAX_TURNS_WITHOUT_MILLS = 40 + 1;

enum class Phase {
    PlacePieces = 1,
    MovePieces = 2,
    GameOver,
    None
};

enum class Piece {
    White,
    Black,
    None
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
    PieceComponent() : id(-1) {}
    PieceComponent(int id, Piece type) : id(id), type(type) {}

    int id;
    Piece type;
    bool in_use = false;
    entt::entity node = entt::null;  // Reference to the node where it sits on

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

struct InactiveTag {
    char inactive;
};

struct NodeComponent {
    NodeComponent() = default;
    NodeComponent(int id) : id(id) {}

    int id;
    entt::entity piece = entt::null;
};

struct ThreefoldRepetitionHistory {
    std::vector<std::array<Piece, 24>> ones;
    std::vector<std::array<Piece, 24>> twos;
};

struct GameStateComponent {
    GameStateComponent() = default;
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

    int turns_without_mills = 0;
    ThreefoldRepetitionHistory repetition_history;
};

namespace undo {
    struct PlacedPiece {
        GameStateComponent state;
        NodeComponent nodes[24];
        PieceComponent pieces[18];  // Some of these will remain uninitialized
        TransformComponent transforms[18];
    };

    struct MovedPiece {
        GameStateComponent state;
        NodeComponent nodes[24];
        PieceComponent pieces[18];  // Some of these will remain uninitialized
        TransformComponent transforms[18];
    };

    struct TakenPiece {
        GameStateComponent state;
        NodeComponent nodes[24];
        PieceComponent pieces[18];  // Some of these will remain uninitialized
        PieceComponent removed_piece;
        entt::entity removed_piece_entity;
        TransformComponent transform;
    };

    enum class MoveType {
        Place, Move, Take
    };

    void remember_place(entt::registry& registry, entt::entity board);
    void remember_move(entt::registry& registry, entt::entity board);
    void remember_take(entt::registry& registry, entt::entity board, entt::entity removed_piece);

    MoveType get_undo_type(entt::registry& registry, entt::entity board);

    PlacedPiece undo_place(entt::registry& registry, entt::entity board);
    MovedPiece undo_move(entt::registry& registry, entt::entity board);
    TakenPiece undo_take(entt::registry& registry, entt::entity board);
}

struct MovesHistoryComponent {
    unsigned int moves = 0;

    std::unordered_map<unsigned int, undo::PlacedPiece> placed_pieces;
    std::unordered_map<unsigned int, undo::MovedPiece> moved_pieces;
    std::unordered_map<unsigned int, undo::TakenPiece> taken_pieces;
};

namespace systems {
    void place_piece(entt::registry& registry, entt::entity board, entt::entity hovered, bool& can_undo);
    void move_pieces(entt::registry& registry, float dt);
    void take_piece(entt::registry& registry, entt::entity board, entt::entity hovered);
    void select_piece(entt::registry& registry, entt::entity board, entt::entity hovered);
    void put_piece(entt::registry& registry, entt::entity board, entt::entity hovered);
    void press(entt::registry& registry, entt::entity board, entt::entity hovered);
    void release(entt::registry& registry, entt::entity board);
    void undo(entt::registry& registry, entt::entity board);
}
