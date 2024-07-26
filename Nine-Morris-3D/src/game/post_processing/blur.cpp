#include "game/post_processing/blur.hpp"

void BlurStep::setup(const sm::PostProcessingCtx& ctx) const {
    bind_texture(ctx.last_texture, 0);
}
