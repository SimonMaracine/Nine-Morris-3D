#include "turn_indicator.hpp"

TurnIndicator::TurnIndicator(std::shared_ptr<sm::GlTexture> white, std::shared_ptr<sm::GlTexture> black)
    : m_white(white), m_black(black) {
    m_quad.texture = white;
}

void TurnIndicator::update(sm::Ctx& ctx, TurnIndicatorType type) {
    switch (type) {
        case TurnIndicatorType::White:
            m_quad.texture = m_white;
            break;
        case TurnIndicatorType::Black:
            m_quad.texture = m_black;
            break;
    }

    m_quad.position = glm::vec2(
        static_cast<float>(ctx.get_window_width()) - static_cast<float>(m_quad.texture->get_width()) * m_quad.scale.x - 20.0f,
        20.0f  // TODO scale + relative offsets
    );

    ctx.add_quad(m_quad);
}
