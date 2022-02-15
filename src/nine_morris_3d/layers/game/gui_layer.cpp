#include <memory>
#include <string>
#include <algorithm>

#include <glm/glm.hpp>

#include "application/events.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/framebuffer.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "other/logging.h"

void GuiLayer::on_bind_layers() {
    game_layer = get_layer<GameLayer>("game");
}

void GuiLayer::on_update(float dt) {
    turn_indicator.update(app->data.width, app->data.height);
    timer.update(app->window->get_time());
}

void GuiLayer::on_draw() {
    render_turn_indicator();
    render_timer();
}

void GuiLayer::render_turn_indicator() {
    if (game_layer->board.turn == Board::Player::White) {
        renderer::draw_quad_2d(turn_indicator.position, turn_indicator.scale,
                app->storage->white_indicator_texture);
    } else {
        renderer::draw_quad_2d(turn_indicator.position, turn_indicator.scale,
                app->storage->black_indicator_texture);
    }
}

void GuiLayer::render_timer() {
    char time[32];
    timer.get_time_formatted(time);

    int width, height;
    app->storage->good_dog_plain_font->get_string_size(time, 1.5f, &width, &height);

    const float x_pos = app->data.width / 2 - width / 2 - 8;
    const float y_pos = app->data.height - height - 50;

    renderer::draw_string_with_shadows(time, glm::vec2(x_pos, y_pos), 1.5f, glm::vec3(0.9f),
            app->storage->good_dog_plain_font);
}
