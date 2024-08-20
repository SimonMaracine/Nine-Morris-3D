#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class NodeObj {
public:
    NodeObj() = default;
    NodeObj(int id, glm::vec3 position, const sm::Renderable& renderable);

    const sm::Renderable& get_renderable() const { return m_renderable; }
    sm::Renderable& get_renderable() { return m_renderable; }
    int get_id() const { return m_id; }
    int get_piece_id() const { return m_piece_id; }

    void set_piece_id(int piece_id) { m_piece_id = piece_id; }
    void set_highlighted(bool highlighted) { m_highlighted = highlighted; }

    void update(sm::Ctx& ctx);
private:
    int m_id {-1};
    int m_piece_id {-1};
    bool m_highlighted {false};

    sm::Renderable m_renderable;
};
