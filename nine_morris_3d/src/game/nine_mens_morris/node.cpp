#include "game/nine_mens_morris/node.hpp"

NodeObj::NodeObj(int id, const sm::Renderable& renderable, glm::vec3 position)
    : HoverableObj(id, renderable) {
    m_renderable.transform.position = position;
    m_renderable.transform.scale = 20.0f;
}

void NodeObj::update(sm::Ctx& ctx) {
    if (m_highlighted) {
        ctx.add_renderable(m_renderable);
    }
}
