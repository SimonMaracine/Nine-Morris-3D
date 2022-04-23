#include <memory>
#include <algorithm>

#include <glm/glm.hpp>

#include "application/events.h"
#include "graphics/renderer/gui_renderer.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "other/logging.h"

void GuiLayer::on_attach() {
    if (!app->data.loaded_turn_indicator) {
        app->data.white_indicator_texture = Texture::create(
            app->assets_data->white_indicator_texture,
            false
        );
        app->data.black_indicator_texture = Texture::create(
            app->assets_data->black_indicator_texture,
            false
        );
        app->data.loaded_turn_indicator = true;  // TODO refactor using on_awake
    }

    std::shared_ptr<gui::Dummy> placeholder = std::make_shared<gui::Dummy>(
        app->gui_renderer->get_frame()
    );
    app->gui_renderer->get_frame()->add(placeholder, 0, 0);

    turn_indicator = std::make_shared<gui::Image>(
        app->gui_renderer->get_frame(), app->data.white_indicator_texture
    );
    app->gui_renderer->get_frame()->add(turn_indicator, 0, 2);

    timer_text = std::make_shared<gui::Text>(
        app->gui_renderer->get_frame(), app->data.good_dog_plain_font,
        "00:00", 1.5f, glm::vec3(0.9f)
    );
    app->gui_renderer->get_frame()->add(timer_text, 0, 1);

    placeholder->padd(glm::ivec2(45, 45), glm::ivec2(0, 0));
    turn_indicator->stick(gui::Sticky::NE)->padd(glm::ivec2(0, 30), glm::ivec2(60, 0));
    timer_text->stick(gui::Sticky::N)->padd(glm::ivec2(0, 0), glm::ivec2(60, 0));
    timer_text->set_shadows(true);
}

void GuiLayer::on_detach() {
    app->gui_renderer->get_frame()->clear();
}

void GuiLayer::on_awake() {
    game_layer = get_layer<GameLayer>("game");
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
