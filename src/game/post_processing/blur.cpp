#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/post_processing/blur.h"

void Blur::render(const PostProcessingContext& context) const {
    shader->bind();

    render_helpers::bind_texture_2d(context.last_texture, 0);
    render_helpers::draw_arrays(6);
}

void Blur::prepare(const PostProcessingContext&) const {
    shader->bind();
    shader->upload_uniform_int("u_screen_texture", 0);
}
