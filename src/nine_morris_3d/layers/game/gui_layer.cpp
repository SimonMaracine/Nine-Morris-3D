#include <memory>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "application/application.h"
#include "application/events.h"
#include "opengl/renderer/renderer.h"
#include "opengl/renderer/framebuffer.h"
#include "other/logging.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/layers/game/game_layer.h"

void GuiLayer::on_attach() {
    setup_quad2d_projection();

    font = std::make_shared<Font>("data/fonts/OpenSans/OpenSans-Regular.ttf", 50.0f, 5, 180, 36, 512);

    font->begin_baking();
    font->bake_characters(32, 127);
    font->end_baking();

    font2 = std::make_shared<Font>("data/fonts/FH-GoodDogPlain-WTT/GOODDP__.TTF", 50.0f, 5, 180, 40, 512);

    font2->begin_baking();
    font2->bake_characters(32, 127);
    font2->end_baking();
}

void GuiLayer::on_detach() {

}

void GuiLayer::on_bind_layers() {

}

void GuiLayer::on_update(float dt) {
    scene->turn_indicator.update(app->data.width, app->data.height);
}

void GuiLayer::on_draw() {
    render_turn_indicator();

    renderer::draw_string("This is a sample of text. And two  spaces.", glm::vec2(200.0f, 200.0f), 1.0f, glm::vec3(0.8f, 0.8f, 1.0f), font);
    renderer::draw_string("How lovely", glm::vec2(100.0f, 100.0f), 1, glm::vec3(0.8f, 0.0f, 0.8f), font2);
    renderer::draw_string("Finally doing some font rendering :D", glm::vec2(10.0f, 10.0f), 1.0f, glm::vec3(1.0f, 1.0f, 1.0f), font2);
    // renderer::draw_string("I will write unknown characters: cămară.", glm::vec2(10.0f, 400.0f), 1.0f, glm::vec3(0.0f, 1.0f, 1.0f), font);
    renderer::draw_string("The headaches were worth it.", glm::vec2(10.0f, 400.0f), 1.0f, glm::vec3(0.0f, 1.0f, 1.0f), font);
}

void GuiLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher (event);

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
    app->storage->orthographic_projection_matrix = glm::ortho(0.0f, (float) event.width, 0.0f, (float) event.height);
    app->storage->upside_down_ortho_projection_matrix = glm::ortho(0.0f, (float) event.width, (float) event.height, 0.0f);
    setup_quad2d_projection();

    return false;
}

void GuiLayer::setup_quad2d_projection() {
    app->storage->quad2d_shader->bind();
    app->storage->quad2d_shader->set_uniform_matrix("u_projection_matrix",
            app->storage->orthographic_projection_matrix);
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
