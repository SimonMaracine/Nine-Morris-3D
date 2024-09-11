#include "game/node.hpp"

NodeObj::NodeObj(int id, glm::vec3 position, const sm::Renderable& renderable)
    : m_id(id), m_renderable(renderable) {
    m_renderable.transform.position = position;
    m_renderable.transform.scale = 20.0f;
}

void NodeObj::update(sm::Ctx& ctx) {
    if (m_highlighted) {
        ctx.add_renderable(m_renderable);
    }
}

void NodeObj::set_renderable(const sm::Renderable& renderable) {
    m_renderable.override_renderable_private(renderable);
}
