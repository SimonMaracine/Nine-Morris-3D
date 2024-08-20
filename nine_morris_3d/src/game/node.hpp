#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class NodeObj {
public:
    NodeObj() = default;
    NodeObj(int id, glm::vec3 position, const sm::Renderable& renderable);

    const sm::Renderable& get_renderable() const { return m_renderable; }
    int get_id() const { return m_id; }
    int get_piece_id() const { return m_piece_id; }

    void update(sm::Ctx& ctx);
private:
    int m_id {};
    int m_piece_id {};
    bool m_highlighted {false};

    sm::Renderable m_renderable;
};
