#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "scenes/loading_scene.h"

void LoadingScene::on_start() {
    // TODO loader here

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

    // TODO loader join

    app->gui_renderer->clear();
}

void LoadingScene::on_update() {
    float width, height, x_pos, y_pos;
    image_configuration(width, height, x_pos, y_pos);

    app->gui_renderer->im_draw_quad(
        glm::vec2(x_pos, y_pos), glm::vec2(width, height), app->res.textures["splash_screen_texture"_hs]
    );

    // if (loader->done_loading()) {
    //     app->change_scene("game");
    // }

    app->change_scene("game");
}

void LoadingScene::image_configuration(float& width, float& height, float& x_pos, float& y_pos) {
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
}
