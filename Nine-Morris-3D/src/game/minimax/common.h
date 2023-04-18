#pragma once

#include <engine/public/application_base.h>  // TODO replace with lighter header

#include "game/game_position.h"
#include "other/constants.h"

enum class MoveType {
    None,
    Place,
    Move,
    PlaceTake,
    MoveTake
};

struct Move {  // TODO can use union
    size_t place_node_index = NULL_INDEX;
    size_t take_node_index = NULL_INDEX;
    size_t move_source_node_index = NULL_INDEX;
    size_t move_destination_node_index = NULL_INDEX;

    PieceType piece = PieceType::None;
    MoveType type = MoveType::None;

    static Move create_place(PieceType piece, size_t place_node_index);
    static Move create_move(PieceType piece, size_t move_source_node_index, size_t move_destination_node_index);
    static Move create_place_take(PieceType piece, size_t place_node_index, size_t take_node_index);
    static Move create_move_take(PieceType piece, size_t move_source_node_index, size_t move_destination_node_index, size_t take_node_index);
};

std::ostream& operator<<(std::ostream& stream, const Move& move);

struct MinimaxAlgorithm {
    MinimaxAlgorithm(Ctx* ctx)
        : ctx(ctx) {}

    using Algorithm = std::function<void(GamePosition, PieceType, Move&, std::atomic<bool>&)>;

    virtual void start(GamePosition position, PieceType piece, Move& result, std::atomic<bool>& running) = 0;

    Ctx* ctx = nullptr;
};
