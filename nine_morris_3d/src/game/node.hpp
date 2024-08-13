#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

class Node {
public:
    Node() = default;
    Node(unsigned int index, glm::vec3 position, const sm::Renderable& renderable);

    const sm::utils::AABB& get_aabb() const { return m_renderable.get_aabb(); }
    unsigned int get_index() const { return m_index; }

    void update(sm::Ctx& ctx);
private:
    unsigned int m_index {};
    bool m_highlighted {false};

    sm::Renderable m_renderable;
};
