#include <memory>
#include <cassert>

#include "application/events.h"
#include "graphics/renderer/renderer.h"
#include "nine_morris_3d/nine_morris_3d.h"
#include "nine_morris_3d/layers/loading/loading_layer.h"
#include "nine_morris_3d/layers/game/game_layer.h"
#include "nine_morris_3d/layers/game/imgui_layer.h"
#include "nine_morris_3d/layers/game/gui_layer.h"
#include "nine_morris_3d/options.h"
#include "other/loader.h"

void LoadingLayer::on_attach() {
    if (app->options.texture_quality == options::NORMAL) {
        if (app->options.skybox == options::FIELD) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::field);
        } else if (app->options.skybox == options::AUTUMN) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::autumn);
        } else {
            assert(false);
        }
    } else if (app->options.texture_quality == options::LOW) {
        if (app->options.skybox == options::FIELD) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::field_low_tex);
        } else if (app->options.skybox == options::AUTUMN) {
            loader = std::make_unique<Loader<AssetsData>>(app->assets_data, assets_data::autumn_low_tex);
        } else {
            assert(false);
        }
    } else {
        assert(false);
    }

    loader->start_loading_thread();

    renderer::disable_stencil();
}

void LoadingLayer::on_detach() {
    DEB_INFO("Done loading assets; initializing the rest of the game...");

    if (loader->get_thread().joinable()) {
        loader->get_thread().detach();
    }

    renderer::enable_stencil();
}

void LoadingLayer::on_update(float dt) {
    if (loader->done_loading()) {
        app->change_scene("game");
    }
}

void LoadingLayer::on_draw() {
    renderer::clear(renderer::Color);

    float width;
    float height;
    float x_pos;
    float y_pos;

    if (static_cast<float>(app->data.width) / static_cast<float>(app->data.height) > 16.0f / 9.0f) {
        width = app->data.width;
        height = app->data.width * (9.0f / 16.0f);
        x_pos = 0.0f;
        y_pos = (height - app->data.height) / -2.0f;
    } else {
        height = app->data.height;
        width = app->data.height * (16.0f / 9.0f);
        x_pos = (width - app->data.width) / -2.0f;
        y_pos = 0.0f;
    }

    renderer::draw_quad_2d(glm::vec2(x_pos, y_pos), glm::vec2(width, height), app->storage->splash_screen_texture);

    renderer::draw_string_with_shadows("Loading...", glm::vec2(app->data.height - 200.0f, 20.0f), 1.2f,
            glm::vec3(0.81f), app->storage->good_dog_plain_font);
}
