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

    loading_text = std::make_shared<gui::Text>(
        app->res.font["good_dog_plain_font"_h], "Loading", 1.2f, glm::vec3(0.81f)
    );
    loading_text->stick(gui::Sticky::SW);
    loading_text->offset(app->data().width - 200, gui::Relative::Left)->offset(20, gui::Relative::Bottom);
    loading_text->set_shadows(true);
    app->gui_renderer->add_widget(loading_text);

    load_splash_screen_texture();

    background = std::make_shared<gui::Image>(app->res.texture["splash_screen_texture"_h]);
    app->gui_renderer->add_widget(background);
}

void LoadingScene::on_stop() {
    DEB_INFO("Done loading assets; initializing the rest of the game...");

    loader->join_and_merge(app->res);
    loader.reset();

    app->gui_renderer->clear();
    background.reset();
    loading_text.reset();
}

void LoadingScene::on_update() {
    float width, height, x_pos, y_pos;
    app->gui_renderer->quad_center(width, height, x_pos, y_pos);

    background->set_position(glm::vec2(x_pos, y_pos));
    background->set_size(glm::vec2(width, height));

    update_loading_animation();

    if (loader->done_loading()) {
        app->change_scene("game");
    }
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
    if (app->get_frames() % 22 == 0) {
        static unsigned int dots = 0;

        std::string text = "Loading";
        text.append(dots, '.');

        loading_text->set_text(text);

        dots++;
        dots %= 4;
    }
}
