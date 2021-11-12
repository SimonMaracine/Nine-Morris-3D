#include <glm/gtc/matrix_transform.hpp>

#include "nine_morris_3d/layers/loading/loading_layer.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"

void LoadingLayer::on_attach() {
    loader.start_loading_thread();
}

void LoadingLayer::on_detach() {
    SPDLOG_INFO("Done loading assets; initializing the rest of the game...");

    if (loader.get_thread().joinable()) {
        loader.get_thread().detach();
    }
}

void LoadingLayer::on_bind_layers() {

}

void LoadingLayer::on_update(float dt) {
    if (loader.done_loading()) {
        app->change_scene(0);
    }
}

void LoadingLayer::on_draw() {
    renderer::clear(renderer::Color);
    renderer::disable_stencil();
    renderer::draw_screen_quad(app->storage->loading_texture->get_id());
    renderer::enable_stencil();
}

void LoadingLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher = Dispatcher(event);
    dispatcher.dispatch<WindowResizedEvent>(WindowResized, BIND(LoadingLayer::on_window_resized));
}

bool LoadingLayer::on_window_resized(events::WindowResizedEvent& event) {
    if (app->storage->scene_framebuffer) {
        app->storage->scene_framebuffer->resize(event.width, event.height);
    }
    app->storage->intermediate_framebuffer->resize(event.width, event.height);
    app->storage->orthographic_projection_matrix = glm::ortho(0.0f, (float) event.width, 0.0f, (float) event.height);

    return false;
}
