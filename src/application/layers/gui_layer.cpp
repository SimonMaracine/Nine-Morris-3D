
#include "application/layers/gui_layer.h"
#include "application/layers/game_layer.h"
#include "application/application.h"
#include "application/events.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/framebuffer.h"
#include "ecs_and_game/systems.h"
#include "other/logging.h"

void GuiLayer::on_attach() {
    active = false;
}

void GuiLayer::on_detach() {
}

void GuiLayer::on_bind_layers() {
    game_layer = get_layer<GameLayer>(0);
}

void GuiLayer::on_update(float dt) {
    systems::turn_indicator(game_layer->registry, application->data.width, application->data.height);
}

void GuiLayer::on_draw() {
    systems::turn_indicator_render(game_layer->registry, game_layer->board, game_layer->storage);
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
