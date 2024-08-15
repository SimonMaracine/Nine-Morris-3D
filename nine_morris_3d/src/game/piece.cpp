#include "game/piece.hpp"

#include <utility>

Piece::Piece(int index, glm::vec3 position, const sm::Renderable& renderable)
    : m_index(index), m_renderable(renderable) {
    m_renderable.transform.position = position;
    m_renderable.transform.scale = 20.0f;
}

void Piece::update(sm::Ctx& ctx) {
    if (m_active) {
        ctx.add_renderable(m_renderable);
    }
}
