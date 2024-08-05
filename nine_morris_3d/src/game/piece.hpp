#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class Piece {
public:
    Piece() = default;
    Piece(unsigned int index, glm::vec3 position, sm::Renderable&& renderable);
private:
    bool active {false};
    unsigned int index {};

    sm::Renderable renderable;
};
