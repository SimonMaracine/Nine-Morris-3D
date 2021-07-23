#pragma once

#include <entt/entt.hpp>

enum class Phase {
    PutPieces,
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

struct NodeComponent {
    Piece type = Piece::None;
    entt::entity piece = entt::null;
};

struct GameStateComponent {
    Phase phase = Phase::PutPieces;
    Player turn = Player::White;
    int white_pieces_count = 0;
    int black_pieces_count = 0;
    entt::entity nodes[24];
};

void game_update_system(entt::registry& registry, entt::entity board, entt::entity hovered);
