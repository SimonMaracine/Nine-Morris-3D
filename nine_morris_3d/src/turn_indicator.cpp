#include "turn_indicator.hpp"

#include "global.hpp"
#include "constants.hpp"

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

    const auto& g {ctx.global<Global>()};

    const float offset {25.0f * g.get_scale()};

    m_quad.position = glm::vec2(
        static_cast<float>(ctx.get_window_width()) - static_cast<float>(m_quad.texture->get_width()) * m_quad.scale.x - offset,
        offset
    );

    m_quad.scale = glm::vec2(
        sm::utils::map(
            static_cast<float>(ctx.get_window_height()),
            static_cast<float>(MIN_HEIGHT),
            static_cast<float>(MAX_HEIGHT),
            0.4f,
            1.0f
        ) * g.get_scale()
    );

    ctx.add_quad(m_quad);
}
