#include <engine/public/graphics.h>
#include <engine/public/external/resmanager.h++>

#include "game/post_processing/combine.h"

void Combine::render(const sm::PostProcessingContext& context) const {
    shader->bind();

    sm::render_helpers::bind_texture_2d(context.original_texture, 0);
    sm::render_helpers::bind_texture_2d(context.last_texture, 1);
    sm::render_helpers::draw_arrays(6);
}

void Combine::prepare(const sm::PostProcessingContext&) const {
    shader->bind();
    shader->upload_uniform_int("u_screen_texture"_H, 0);
    shader->upload_uniform_int("u_bright_texture"_H, 1);
    shader->upload_uniform_float("u_strength"_H, strength);
}
