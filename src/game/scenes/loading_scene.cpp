#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/scenes/loading_scene.h"
#include "game/assets_load.h"
#include "game/assets.h"
#include "launcher/launcher_options.h"
#include "other/data.h"

void LoadingScene::on_start() {
    auto& data = app->user_data<Data>();

    loader = std::make_unique<assets_load::CustomLoader>(assets_load::all_start);
    loader->start_loading_thread(
        data.launcher_options.normal_mapping,
        data.launcher_options.texture_quality,
        data.options.labeled_board,
        data.options.skybox
    );

    setup_widgets();
    load_splash_screen_texture();

    auto background = data.image_cache.load("background"_h, app->res.texture["splash_screen_texture"_h]);
    app->gui_renderer->add_widget(background);

    loading_animation.previous_seconds = app->window->get_time();
}

void LoadingScene::on_stop() {
    auto& data = app->user_data<Data>();

    DEB_INFO("Done loading assets; initializing the rest of the game...");

    loader->join_and_merge(app->res);
    loader.reset();

    app->gui_renderer->clear();
    data.image_cache.clear();
    data.text_cache.clear();
}

void LoadingScene::on_update() {
    auto& data = app->user_data<Data>();

    float width, height, x_pos, y_pos;
    app->gui_renderer->quad_center(width, height, x_pos, y_pos);

    data.image_cache["background"_h]->set_position(glm::vec2(x_pos, y_pos));
    data.image_cache["background"_h]->set_size(glm::vec2(width, height));

    update_loading_animation();

    if (loader->done_loading()) {
        app->change_scene("game");
    }
}

void LoadingScene::setup_widgets() {
    auto& data = app->user_data<Data>();

    constexpr int LOWEST_RESOLUTION = 288;
    constexpr int HIGHEST_RESOLUTION = 1035;

    auto loading_text = data.text_cache.load(
        "loading_text"_h,
        app->res.font["good_dog_plain_font"_h],
        "Loading", 1.5f, glm::vec3(0.81f)
    );
    loading_text->stick(gui::Sticky::SE);
    loading_text->offset(20, gui::Relative::Right);
    loading_text->offset(20, gui::Relative::Bottom);
    loading_text->scale(0.4f, 1.4f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);  // TODO tweak parameters

    const auto size = loading_text->get_actual_size();
    loading_text->fake_size(glm::vec2(size.x + 80.0f, size.y));
    loading_text->set_shadows(true);
    app->gui_renderer->add_widget(loading_text);
}

void LoadingScene::load_splash_screen_texture() {
    using namespace assets;
    using namespace encrypt;
    using namespace paths;

    TextureSpecification specification;
    specification.min_filter = Filter::Linear;
    specification.mag_filter = Filter::Linear;

    app->res.texture.load("splash_screen_texture"_h, encr(path_for_assets(SPLASH_SCREEN_TEXTURE)), specification);
}

void LoadingScene::update_loading_animation() {
    auto& data = app->user_data<Data>();

    const double current_seconds = app->window->get_time();
    const double elapsed_seconds = current_seconds - loading_animation.previous_seconds;
    loading_animation.previous_seconds = current_seconds;

    loading_animation.total_time += elapsed_seconds;

    if (loading_animation.total_time > 0.4) {
        loading_animation.total_time = 0.0;

        std::string text = "Loading";
        text.append(loading_animation.dots, '.');

        data.text_cache["loading_text"_h]->set_text(text);

        loading_animation.dots++;
        loading_animation.dots %= 6;
    }
}
