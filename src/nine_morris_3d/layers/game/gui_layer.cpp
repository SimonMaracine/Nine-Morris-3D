#include <glm/glm.hpp>

#include "application/events.h"
#include "graphics/renderer/gui_renderer.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/layers/game/game_layer.h"

void GuiLayer::on_attach() {
    turn_indicator = std::make_shared<gui::Image>(app->data.white_indicator_texture);
    turn_indicator->stick(gui::Sticky::NE);
    turn_indicator->offset(50, gui::Relative::Right)->offset(50, gui::Relative::Top);
    turn_indicator->scale(0.4f, 1.0f, 288, 1035);
    app->gui_renderer->add_widget(turn_indicator);

    timer_text = std::make_shared<gui::Text>(app->data.good_dog_plain_font, "00:00", 1.5f, glm::vec3(0.9f));
    timer_text->stick(gui::Sticky::N);
    timer_text->offset(60, gui::Relative::Top);
    timer_text->scale(0.6f, 1.4f, 288, 1035);
    timer_text->set_shadows(true);

    if (!app->options.hide_timer) {
        app->gui_renderer->add_widget(timer_text);
    }
}

void GuiLayer::on_detach() {
    app->gui_renderer->clear();
}

void GuiLayer::on_awake() {
    game_layer = get_layer<GameLayer>("game");

    app->data.white_indicator_texture = Texture::create(
        app->assets_data->white_indicator_texture, false
    );
    app->data.black_indicator_texture = Texture::create(
        app->assets_data->black_indicator_texture, false
    );
}

void GuiLayer::on_update(float dt) {
    timer.update(app->window->get_time());

    char time[32];
    timer.get_time_formatted(time);
    timer_text->set_text(time);

    if (game_layer->board.turn == Board::Player::White) {
        turn_indicator->set_image(app->data.white_indicator_texture);
    } else {
        turn_indicator->set_image(app->data.black_indicator_texture);
    }
}
