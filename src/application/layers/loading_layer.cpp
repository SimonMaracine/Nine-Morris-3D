#include "application/layers/loading_layer.h"

void LoadingLayer::on_attach() {
    
}

void LoadingLayer::on_detach() {

}

void LoadingLayer::on_bind_layers() {
    game_layer = (GameLayer*) get_layer(2);
    gui_layer = (GuiLayer*) get_layer(1);
}

void LoadingLayer::on_update(float dt) {
    if (game_layer->loader->done_loading()) {
        active = false;
        game_layer->active = true;
        gui_layer->active = true;
    }
}

void LoadingLayer::on_draw() {
    renderer::clear(renderer::Color);
    renderer::draw_loading();
}

void LoadingLayer::on_event(events::Event& event) {

}
