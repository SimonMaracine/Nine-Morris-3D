#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "game/hoverable.hpp"

class NodeObj : public HoverableObj {
public:
    NodeObj() = default;
    NodeObj(int id, const sm::Renderable& renderable, glm::vec3 position);

    void set_highlighted(bool highlighted) { m_highlighted = highlighted; }

    void update(sm::Ctx& ctx);

    int piece_id {-1};
private:
    bool m_highlighted {false};
};
