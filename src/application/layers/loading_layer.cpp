#include "application/layers/loading_layer.h"
#include "application/layers/game_layer.h"
#include "application/layers/imgui_layer.h"
#include "application/layers/gui_layer.h"

void LoadingLayer::on_attach() {
    application->asset_manager.require(0);
    application->asset_manager.require(1);
    application->asset_manager.require(2);
    application->asset_manager.require(3);
    application->asset_manager.require(4);
    application->asset_manager.require(5);
    application->asset_manager.require(7);
    application->asset_manager.require(9);
    application->asset_manager.require(10);
    application->asset_manager.require(12);
    application->asset_manager.require(14);
    application->asset_manager.require(16);
    application->asset_manager.require(17);
    application->asset_manager.require(18);
    application->asset_manager.require(19);
    application->asset_manager.require(20);
    application->asset_manager.require(21);
    auto required = application->asset_manager.get_required();

    loader.start_loading_thread(required);

    application->asset_manager.load_now(11, AssetType::TextureData);
}

void LoadingLayer::on_detach() {

}

void LoadingLayer::on_bind_layers() {
    game_layer = get_layer<GameLayer>(0);
    imgui_layer = get_layer<ImGuiLayer>(2);
    gui_layer = get_layer<GuiLayer>(1);
}

void LoadingLayer::on_update(float dt) {
    if (loader.done_loading()) {
        active = false;
        game_layer->active = true;
        imgui_layer->active = true;
        gui_layer->active = true;
    }
}

void LoadingLayer::on_draw() {
    renderer::clear(renderer::Color);
    // renderer::draw_loading();
}

void LoadingLayer::on_event(events::Event& event) {

}
