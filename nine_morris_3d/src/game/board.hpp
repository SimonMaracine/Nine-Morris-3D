#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class Board {
public:
    Board() = default;
    virtual ~Board() = default;

    Board(const Board&) = default;
    Board& operator=(const Board&) = default;
    Board(Board&&) = default;
    Board& operator=(Board&&) = default;

    virtual void set_board_paint_renderable(const sm::Renderable& board_paint) = 0;
};
