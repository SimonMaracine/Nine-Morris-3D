#include <memory>
#include <cassert>

#include <glm/gtc/matrix_transform.hpp>

#include "application/events.h"
#include "opengl/renderer/renderer.h"
#include "other/loader.h"
#include "nine_morris_3d/layers/loading/loading_layer.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/options.h"

void LoadingLayer::on_attach() {
    if (scene->options.texture_quality == options::NORMAL) {
        if (scene->options.skybox == options::FIELD) {
            loader = std::make_unique<Loader<AssetsLoad>>(app->assets_load, assets_load::field);
        } else if (scene->options.skybox == options::AUTUMN) {
            loader = std::make_unique<Loader<AssetsLoad>>(app->assets_load, assets_load::autumn);
        } else {
            assert(false);
        }
    } else if (scene->options.texture_quality == options::LOW) {
        if (scene->options.skybox == options::FIELD) {
            loader = std::make_unique<Loader<AssetsLoad>>(app->assets_load, assets_load::field_low_tex);
        } else if (scene->options.skybox == options::AUTUMN) {
            loader = std::make_unique<Loader<AssetsLoad>>(app->assets_load, assets_load::autumn_low_tex);
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }

    loader->start_loading_thread();
}

void LoadingLayer::on_detach() {
    SPDLOG_INFO("Done loading assets; initializing the rest of the game...");

    if (loader->get_thread().joinable()) {
        loader->get_thread().detach();
    }
}

void LoadingLayer::on_bind_layers() {

}

void LoadingLayer::on_update(float dt) {
    if (loader->done_loading()) {
        app->change_scene(0);
    }
}

void LoadingLayer::on_draw() {
    renderer::clear(renderer::Color);
    renderer::disable_stencil();
    renderer::draw_screen_quad(app->storage->splash_screen_texture->get_id());
    renderer::enable_stencil();
}

void LoadingLayer::on_event(events::Event& event) {
    using namespace events;

    Dispatcher dispatcher (event);
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
