#include "application/scenes/loading/loading_layer.h"
#include "application/scenes/game/game_layer.h"
#include "application/scenes/game/imgui_layer.h"
#include "application/scenes/game/gui_layer.h"

void LoadingLayer::on_attach() {
    app->asset_manager.load_now(11, AssetType::TextureFlipped);
    app->storage->loading_texture = Texture::create(app->asset_manager.get_texture_flipped(11), false);

    app->asset_manager.require(0);
    app->asset_manager.require(1);
    app->asset_manager.require(2);
    app->asset_manager.require(3);
    app->asset_manager.require(4);

    if (scene->options.texture_quality == 0) {
        app->asset_manager.require(5);
        app->asset_manager.require(7);
        app->asset_manager.require(12);
        app->asset_manager.require(14);
    } else {
        app->asset_manager.require(6);
        app->asset_manager.require(8);
        app->asset_manager.require(13);
        app->asset_manager.require(15);
    }

    app->asset_manager.require(9);
    app->asset_manager.require(10);
    app->asset_manager.require(16);
    app->asset_manager.require(17);
    app->asset_manager.require(18);
    app->asset_manager.require(19);
    app->asset_manager.require(20);
    app->asset_manager.require(21);
    auto required = app->asset_manager.get_required();

    loader.start_loading_thread(required);
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

}
