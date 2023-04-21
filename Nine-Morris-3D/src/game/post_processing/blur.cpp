#include <engine/public/graphics.h>
#include <engine/public/external/resmanager.h++>

#include "game/post_processing/blur.h"

void Blur::render(const sm::PostProcessingContext& context) const {
    shader->bind();

    sm::render_helpers::bind_texture_2d(context.last_texture, 0);
    sm::render_helpers::draw_arrays(6);
}

void Blur::prepare(const sm::PostProcessingContext&) const {
    shader->bind();
    shader->upload_uniform_int("u_screen_texture"_H, 0);
}
