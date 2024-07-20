#include "game/post_processing/blur.hpp"

void BlurStep::setup(const sm::PostProcessingCtx& ctx) const {
    sm::opengl::bind_texture_2d(ctx.last_texture, 0);
}
