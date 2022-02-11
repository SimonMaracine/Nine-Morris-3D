#include <memory>
#include <string>
#include <algorithm>
#include <string.h>

#include <glm/glm.hpp>

#include "application/application.h"
#include "application/app.h"
#include "application/events.h"
#include "graphics/renderer/renderer.h"
#include "graphics/renderer/framebuffer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "other/logging.h"

void GuiLayer::on_update(float dt) {
    scene->turn_indicator.update(app->data.width, app->data.height);
    scene->timer.update(app->window->get_time());
}

void GuiLayer::on_draw() {
    render_turn_indicator();
    render_timer();
}

void GuiLayer::render_turn_indicator() {
    if (scene->board.turn == Board::Player::White) {
        renderer::draw_quad_2d(scene->turn_indicator.position, scene->turn_indicator.scale,
                app->storage->white_indicator_texture);
    } else {
        renderer::draw_quad_2d(scene->turn_indicator.position, scene->turn_indicator.scale,
                app->storage->black_indicator_texture);
    }
}

void GuiLayer::render_timer() {
    const unsigned int minutes = scene->timer.get_time_seconds() / 60;
    const unsigned int seconds = scene->timer.get_time_seconds() % 60;    

    char time[32];
    sprintf(time, "%.2u:%.2u", minutes, seconds);

    int width, height;
    app->storage->good_dog_plain_font->get_string_size(time, 1.5f, &width, &height);

    const float x_pos = app->data.width / 2 - width / 2 - 8;
    const float y_pos = app->data.height - height - 50;

    renderer::draw_string_with_shadows(time, glm::vec2(x_pos, y_pos), 1.5f, glm::vec3(0.9f), app->storage->good_dog_plain_font);
}
