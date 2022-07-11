#include "graphics/renderer/renderer.h"
#include "nine_morris_3d/post_processing/bright_filter.h"

void BrightFilter::render(const PostProcessingContext& context) const {
    shader->bind();

    render_helpers::bind_texture_2d(context.last_framebuffer, 0);
    render_helpers::draw_arrays(6);
}

void BrightFilter::prepare(const PostProcessingContext& context) const {
    shader->bind();
    shader->upload_uniform_int("u_screen_texture", 0);
}
