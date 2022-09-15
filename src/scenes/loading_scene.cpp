#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "scenes/loading_scene.h"
#include "game/nine_morris_3d.h"
#include "game/options.h"
#include "game/assets_load_functions.h"

void LoadingScene::on_start() {
    auto& data = app->user_data<Data>();

    loader = std::make_unique<ConcurrentLoader<options::Options>>(data.res_thread, assets_load_functions::all_start);
    loader->start_loading_thread(data.options);

    auto loading_text = std::make_shared<gui::Text>(
        app->res.fonts["good_dog_plain_font"_hs], "Loading...", 1.2f, glm::vec3(0.81f)
    );
    loading_text->stick(gui::Sticky::SE);
    loading_text->offset(22, gui::Relative::Right)->offset(20, gui::Relative::Bottom);
    loading_text->set_shadows(true);
    app->gui_renderer->add_widget(loading_text);
}

void LoadingScene::on_stop() {
    DEB_INFO("Done loading assets; initializing the rest of the game...");

    loader->join();
    loader = nullptr;

    app->gui_renderer->clear();
}

void LoadingScene::on_update() {
    float width, height, x_pos, y_pos;
    quad_configuration(width, height, x_pos, y_pos);

    app->gui_renderer->im_draw_quad(
        glm::vec2(x_pos, y_pos), glm::vec2(width, height), app->res.textures["splash_screen_texture"_hs]
    );

    if (loader->done_loading()) {
        app->change_scene("game");
    }
}

void LoadingScene::quad_configuration(float& width, float& height, float& x_pos, float& y_pos) {
    if (static_cast<float>(app->data().width) / app->data().height > 16.0f / 9.0f) {
        width = app->data().width;
        height = app->data().width * (9.0f / 16.0f);
        x_pos = 0.0f;
        y_pos = (height - app->data().height) / -2.0f;
    } else {
        height = app->data().height;
        width = app->data().height * (16.0f / 9.0f);
        x_pos = (width - app->data().width) / -2.0f;
        y_pos = 0.0f;
    }
}
