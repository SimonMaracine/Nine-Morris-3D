#pragma once

#include <engine/public/audio.h>
#include <engine/public/scene.h>

#include "game/piece_movement.h"
#include "other/constants.h"

struct Piece {
    Piece() = default;
    Piece(size_t index, PieceType type, sm::renderables::Model* model, std::shared_ptr<sm::al::Source> source)
        : index(index), type(type), model(model), source(source) {}
    ~Piece() = default;

    Piece(const Piece&) = delete;
    Piece& operator=(const Piece&) = default;
    Piece(Piece&&) noexcept = default;
    Piece& operator=(Piece&&) noexcept = default;

    size_t index = NULL_INDEX;  // From 0 through 17 on standard game

    PieceType type = PieceType::None;
    bool in_use = false;  // true, if it is on the board

    sm::renderables::Model* model = nullptr;
    std::shared_ptr<sm::al::Source> source;

    size_t node_index = NULL_INDEX;  // Reference to the node on top of which it sits on

    PieceMovement movement;

    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
    bool selected = false;
};
