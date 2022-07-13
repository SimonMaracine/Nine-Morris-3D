#include <glm/glm.hpp>

#include "application/events.h"
#include "graphics/renderer/gui_renderer.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/game_context.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/layers/game/game_layer.h"

void GuiLayer::on_attach() {
    constexpr int LOWEST_RESOLUTION = 288;
    constexpr int HIGHEST_RESOLUTION = 1035;

    turn_indicator = std::make_shared<gui::Image>(app->data.white_indicator_texture);
    turn_indicator->stick(gui::Sticky::SE);
    turn_indicator->offset(30, gui::Relative::Right)->offset(30, gui::Relative::Bottom);
    turn_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
    app->gui_renderer->add_widget(turn_indicator);

    timer_text = std::make_shared<gui::Text>(app->data.good_dog_plain_font, "00:00", 1.5f, glm::vec3(0.9f));
    timer_text->stick(gui::Sticky::N);
    timer_text->offset(60, gui::Relative::Top);
    timer_text->scale(0.6f, 1.4f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
    timer_text->set_shadows(true);

    if (!app->options.hide_timer) {
        app->gui_renderer->add_widget(timer_text);
    }

    wait_indicator = std::make_shared<gui::Image>(app->data.wait_indicator_texture);
    wait_indicator->stick(gui::Sticky::NE);
    wait_indicator->offset(25, gui::Relative::Right)->offset(55, gui::Relative::Top);
    wait_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);

    computer_thinking_indicator = std::make_shared<gui::Image>(t);
    computer_thinking_indicator->stick(gui::Sticky::NE);
    computer_thinking_indicator->offset(25, gui::Relative::Right)->offset(55, gui::Relative::Top);
    computer_thinking_indicator->scale(0.4f, 1.0f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);
}

void GuiLayer::on_detach() {
    app->gui_renderer->clear();
}

void GuiLayer::on_awake() {
    game_layer = get_layer<GameLayer>("game");

    TextureSpecification specification;
    specification.min_filter = Filter::Linear;
    specification.mag_filter = Filter::Linear;

    app->data.wait_indicator_texture = Texture::create(
        app->assets_data->wait_indicator_texture, specification
    );
    app->data.white_indicator_texture = Texture::create(
        app->assets_data->white_indicator_texture, specification
    );
    app->data.black_indicator_texture = Texture::create(
        app->assets_data->black_indicator_texture, specification
    );
    t = Texture::create("data/textures/indicator/computer_thinking_indicator.png", specification);
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

    if (!game_layer->board.next_move) {
        if (!show_wait_indicator) {
            app->gui_renderer->add_widget(wait_indicator);
            show_wait_indicator = true;
        }
    } else {
        if (show_wait_indicator) {
            app->gui_renderer->remove_widget(wait_indicator);
            show_wait_indicator = false;
        }
    }

    if (game_layer->game.state == GameState::ComputerThinkingMove) {
        if (!show_computer_thinking_indicator) {
            app->gui_renderer->add_widget(computer_thinking_indicator);
            show_computer_thinking_indicator = true;
        }
    } else {
        if (show_computer_thinking_indicator) {
            app->gui_renderer->remove_widget(computer_thinking_indicator);
            show_computer_thinking_indicator = false;
        }
    }
}
