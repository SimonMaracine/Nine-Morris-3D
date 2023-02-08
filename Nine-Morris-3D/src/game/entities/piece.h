#pragma once

#include <engine/engine_audio.h>
#include <engine/engine_scene.h>

#include "game/piece_movement.h"
#include "other/constants.h"

struct Piece {
    Piece() = default;
    Piece(Index index, PieceType type, object::Model* model, std::shared_ptr<al::Source> source)
        : index(index), type(type), model(model), source(source) {}
    ~Piece() = default;

    Piece(const Piece&) = delete;
    Piece& operator=(const Piece&) = default;
    Piece(Piece&&) = default;
    Piece& operator=(Piece&&) = default;

    Index index = NULL_INDEX;  // From 0 through 17 on standard game

    PieceType type = PieceType::None;
    bool in_use = false;  // true, if it is on the board

    object::Model* model = nullptr;
    std::shared_ptr<al::Source> source;

    Index node_index = NULL_INDEX;  // Reference to the node on top of which it sits on

    PieceMovement movement;

    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
    bool selected = false;
};
