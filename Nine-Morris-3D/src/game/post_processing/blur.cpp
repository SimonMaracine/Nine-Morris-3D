#include "game/post_processing/blur.hpp"

void BlurStep::setup(const sm::PostProcessingContext& context) const {
    sm::opengl::bind_texture_2d(context.last_texture, 0);
}
