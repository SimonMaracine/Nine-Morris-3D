#include <nine_morris_3d_engine/engine_graphics.h>

#include "game/post_processing/bright_filter.h"

void BrightFilter::render(const PostProcessingContext& context) const {
    shader->bind();

    render_helpers::bind_texture_2d(context.last_texture, 0);
    render_helpers::draw_arrays(6);
}

void BrightFilter::prepare(const PostProcessingContext&) const {
    shader->bind();
    shader->upload_uniform_int("u_screen_texture", 0);
}
