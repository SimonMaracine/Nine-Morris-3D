#include "graphics/renderer/renderer.h"
#include "nine_morris_3d/post_processing/combine.h"

void Combine::render(const PostProcessingContext& context) const {
    shader->bind();

    render_helpers::bind_texture_2d(screen_texture, 0);
    render_helpers::bind_texture_2d(context.last_framebuffer, 1);
    render_helpers::draw_arrays(6);
}

void Combine::prepare(const PostProcessingContext& context) const {
    shader->bind();
    shader->upload_uniform_int("u_screen_texture", 0);
    shader->upload_uniform_int("u_bright_texture", 1);
}
