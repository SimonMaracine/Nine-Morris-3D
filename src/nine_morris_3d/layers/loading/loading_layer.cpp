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
#include "other/assert.h"

void LoadingLayer::on_attach() {
    loader = std::make_unique<Loader<AssetsData, options::Options>>(app->assets_data, assets_data::all_start);
    loader->start_loading_thread(app->options);

    std::shared_ptr<gui::Text> loading_text = std::make_shared<gui::Text>(
        app->data.good_dog_plain_font, "Loading...", 1.2f, glm::vec3(0.81f)
    );
    loading_text->stick(gui::Sticky::SE);
    loading_text->offset(22, gui::Relative::Right)->offset(20, gui::Relative::Bottom);
    loading_text->set_shadows(true);
    app->gui_renderer->add_widget(loading_text);
}

void LoadingLayer::on_detach() {
    DEB_INFO("Done loading assets; initializing the rest of the game...");

    loader->get_thread().join();
    loader = nullptr;

    app->gui_renderer->clear();
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
