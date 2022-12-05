#pragma once

#include <nine_morris_3d_engine/engine_audio.h>
#include <nine_morris_3d_engine/engine_graphics.h>

#include "game/piece_movement.h"
#include "other/constants.h"

struct Piece {
    Piece() = default;
    Piece(size_t index, PieceType type, std::shared_ptr<Renderer::Model> model, std::shared_ptr<al::Source> source)
        : index(index), type(type), model(model), source(source) {}
    ~Piece() = default;

    Piece(const Piece&) = delete;
    Piece& operator=(const Piece&) = default;
    Piece(Piece&&) = default;
    Piece& operator=(Piece&&) = default;

    size_t index = NULL_INDEX;  // From 0 through 17 on standard game

    PieceType type = PieceType::None;
    bool in_use = false;  // true, if it is on the board

    std::shared_ptr<Renderer::Model> model;
    std::shared_ptr<al::Source> source;

    size_t node_index = NULL_INDEX;  // Reference to the node on top of which it sits on

    PieceMovement movement;

    bool show_outline = false;
    bool to_take = false;
    bool pending_remove = false;
    bool selected = false;
};
