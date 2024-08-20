#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

enum class PieceType {
    White = 1,
    Black = 2
};

class PieceObj {
public:
    PieceObj() = default;
    PieceObj(int id, glm::vec3 position, const sm::Renderable& renderable, PieceType type);

    const sm::Renderable& get_renderable() const { return m_renderable; }
    int get_id() const { return m_id; }
    int get_node_id() const { return m_node_id; }
    PieceType get_type() const { return m_type; }

    void update(sm::Ctx& ctx);
private:
    int m_id {};
    int m_node_id {};
    PieceType m_type {};
    bool m_active {true};

    sm::Renderable m_renderable;
};
