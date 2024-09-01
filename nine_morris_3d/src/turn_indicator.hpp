#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

enum class TurnIndicatorType {
    White = 1,
    Black = 2
};

class TurnIndicator {
public:
    TurnIndicator() = default;
    TurnIndicator(std::shared_ptr<sm::GlTexture> white, std::shared_ptr<sm::GlTexture> black);

    void update(sm::Ctx& ctx, TurnIndicatorType type);
private:
    std::shared_ptr<sm::GlTexture> m_white;
    std::shared_ptr<sm::GlTexture> m_black;

    sm::Quad m_quad;
};
