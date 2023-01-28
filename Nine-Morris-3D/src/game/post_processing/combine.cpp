#include <engine/engine_graphics.h>

#include "game/post_processing/combine.h"

void Combine::render(const PostProcessingContext& context) const {
    shader->bind();

    render_helpers::bind_texture_2d(context.original_texture, 0);
    render_helpers::bind_texture_2d(context.last_texture, 1);
    render_helpers::draw_arrays(6);
}

void Combine::prepare(const PostProcessingContext&) const {
    shader->bind();
    shader->upload_uniform_int("u_screen_texture"_H, 0);
    shader->upload_uniform_int("u_bright_texture"_H, 1);
    shader->upload_uniform_float("u_strength"_H, strength);
}
