#pragma once

#include <array>

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/node.hpp"
#include "game/piece.hpp"

class StandardBoard {
public:
    StandardBoard() = default;
    StandardBoard(sm::Renderable&& renderable);

    void update(sm::Ctx& ctx);
private:
    std::array<Node, 24> nodes {};
    std::array<Piece, 18> pieces {};

    sm::Renderable renderable;
    sm::Renderable paint_renderable;
};
