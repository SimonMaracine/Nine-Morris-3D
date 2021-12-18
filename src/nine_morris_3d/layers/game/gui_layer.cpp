
#include "application/application.h"
#include "application/events.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/framebuffer.h"
#include "other/logging.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/layers/game/game_layer.h"

void GuiLayer::on_attach() {

}

void GuiLayer::on_detach() {

}

void GuiLayer::on_bind_layers() {
    game_layer = get_layer<GameLayer>(0, scene);
}

void GuiLayer::on_update(float dt) {
    scene->turn_indicator.update(app->data.width, app->data.height);
}

void GuiLayer::on_draw() {
    render_turn_indicator();
}

void GuiLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher = Dispatcher(event);

    dispatcher.dispatch<MouseScrolledEvent>(MouseScrolled, BIND(GuiLayer::on_mouse_scrolled));
    dispatcher.dispatch<MouseMovedEvent>(MouseMoved, BIND(GuiLayer::on_mouse_moved));
    dispatcher.dispatch<MouseButtonPressedEvent>(MouseButtonPressed, BIND(GuiLayer::on_mouse_button_pressed));
    dispatcher.dispatch<MouseButtonReleasedEvent>(MouseButtonReleased, BIND(GuiLayer::on_mouse_button_released));
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(GuiLayer::on_window_resized));
}

bool GuiLayer::on_mouse_scrolled(events::MouseScrolledEvent& event) {
    return false;
}

bool GuiLayer::on_mouse_moved(events::MouseMovedEvent& event) {
    return false;
}

bool GuiLayer::on_mouse_button_pressed(events::MouseButtonPressedEvent& event) {
    return false;
}

bool GuiLayer::on_mouse_button_released(events::MouseButtonReleasedEvent& event) {
    return false;
}

bool GuiLayer::on_window_resized(events::WindowResizedEvent& event) {
    return false;
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
