#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class Piece {
public:
    Piece() = default;
    Piece(int index, glm::vec3 position, const sm::Renderable& renderable);

    const sm::Renderable& get_renderable() const { return m_renderable; }
    int get_index() const { return m_index; }

    void update(sm::Ctx& ctx);
private:
    int m_index {};
    bool m_active {true};

    sm::Renderable m_renderable;
};
