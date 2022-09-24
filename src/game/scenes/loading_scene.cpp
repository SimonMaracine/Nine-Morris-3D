#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/scenes/loading_scene.h"
#include "game/game_options.h"
#include "game/assets_load_functions.h"
#include "game/assets.h"
#include "other/data.h"

void LoadingScene::on_start() {
    auto& data = app->user_data<Data>();

    loader = std::make_unique<ConcurrentLoader<game_options::GameOptions>>(assets_load_functions::all_start);
    loader->start_loading_thread(data.options);

    auto loading_text = std::make_shared<gui::Text>(
        app->res.font["good_dog_plain_font"_h], "Loading...", 1.2f, glm::vec3(0.81f)
    );
    loading_text->stick(gui::Sticky::SE);
    loading_text->offset(22, gui::Relative::Right)->offset(20, gui::Relative::Bottom);
    loading_text->set_shadows(true);
    app->gui_renderer->add_widget(loading_text);

    using namespace assets;
    using namespace encrypt;
    using namespace paths;

    // Load splash screen
    TextureSpecification specification;
    specification.min_filter = Filter::Linear;
    specification.mag_filter = Filter::Linear;

    app->res.texture.load("splash_screen_texture"_h, encr(path_for_assets(SPLASH_SCREEN_TEXTURE)), specification);
}

void LoadingScene::on_stop() {
    DEB_INFO("Done loading assets; initializing the rest of the game...");

    loader->join_and_merge(app->res);
    loader.reset();

    app->gui_renderer->clear();
}

void LoadingScene::on_update() {
    float width, height, x_pos, y_pos;
    app->gui_renderer->quad_center(width, height, x_pos, y_pos);

    app->gui_renderer->im_draw_quad(
        glm::vec2(x_pos, y_pos), glm::vec2(width, height), app->res.texture["splash_screen_texture"_h]
    );

    if (loader->done_loading()) {
        app->change_scene("game");
    }
}
