#include <memory>
#include <cassert>

#include <glm/glm.hpp>

#include "application/events.h"
#include "graphics/renderer/gui_renderer.h"
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

    std::shared_ptr<gui::Text> loading_text = std::make_shared<gui::Text>(
        app->gui_renderer->get_frame(), app->data.good_dog_plain_font,
        "Loading...", 1.2f, glm::vec3(0.81f)
    );
    app->gui_renderer->get_frame()->add(loading_text, 0, 0);
    loading_text->stick(gui::Sticky::SE)->padd(glm::ivec2(0, 25), glm::ivec2(0, 20));
}

void LoadingLayer::on_detach() {
    DEB_INFO("Done loading assets; initializing the rest of the game...");

    if (loader->get_thread().joinable()) {
        loader->get_thread().detach();
    }

    app->gui_renderer->get_frame()->clear();
}

void LoadingLayer::on_update(float dt) {
    float width;
    float height;
    float x_pos;
    float y_pos;

    if (static_cast<float>(app->app_data.width) / app->app_data.height > 16.0f / 9.0f) {
        width = app->app_data.width;
        height = app->app_data.width * (9.0f / 16.0f);
        x_pos = 0.0f;
        y_pos = (height - app->app_data.height) / -2.0f;
    } else {
        height = app->app_data.height;
        width = app->app_data.height * (16.0f / 9.0f);
        x_pos = (width - app->app_data.width) / -2.0f;
        y_pos = 0.0f;
    }

    app->gui_renderer->im_draw_quad(glm::vec2(x_pos, y_pos), glm::vec2(width, height),
            app->data.splash_screen_texture);

    if (loader->done_loading()) {
        app->change_scene("game");
    }
}
