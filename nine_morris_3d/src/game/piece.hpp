#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class Piece {
public:
    Piece() = default;
    Piece(unsigned int index, glm::vec3 position, const sm::Renderable& renderable);

    void update(sm::Ctx& ctx);
private:
    unsigned int m_index {};
    bool m_active {true};

    sm::Renderable m_renderable;
};
