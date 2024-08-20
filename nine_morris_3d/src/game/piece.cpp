#include "game/piece.hpp"

#include <utility>

PieceObj::PieceObj(int id, glm::vec3 position, const sm::Renderable& renderable, PieceType type)
    : m_id(id), m_type(type), m_renderable(renderable) {
    m_renderable.transform.position = position;
    m_renderable.transform.scale = 20.0f;
}

void PieceObj::update(sm::Ctx& ctx) {
    if (m_active) {
        ctx.add_renderable(m_renderable);
    }
}
