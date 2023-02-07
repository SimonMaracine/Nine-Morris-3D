#include <engine/engine_graphics.h>
#include <engine/engine_other.h>

#include "game/scenes/loading_scene.h"
#include "game/scenes/common.h"
#include "game/assets_load.h"
#include "game/assets.h"
#include "launcher/launcher_options.h"
#include "other/data.h"

void LoadingScene::on_start() {
    auto& data = app->user_data<Data>();

    load_splash_screen_texture();

    loader = std::make_unique<assets_load::AllStartLoader>(
        [this, &data]() {
            app->change_scene(scene_int_to_id(data.options.scene));
        }
    );

    loader->start_loading_thread(
        data.launcher_options.normal_mapping,
        data.launcher_options.texture_quality,
        data.options.labeled_board,
        data.options.skybox
    );

    setup_widgets();

    auto background = app->res.image.load("background"_H, app->res.texture["splash_screen"_H]);
    app->gui_renderer->add_widget(background);

    loading_animation.previous_seconds = app->window->get_time();
}

void LoadingScene::on_stop() {
    DEB_INFO("Done loading assets; initializing the rest of the game...");

    if (app->running) {
        // Initialize only if game is not closing
        initialize_game(app);
    }

    loader.reset();

    app->gui_renderer->clear();
    app->res.image.clear();
    app->res.text.clear();
}

void LoadingScene::on_update() {
    float width, height, x_pos, y_pos;
    app->gui_renderer->quad_center(width, height, x_pos, y_pos);

    app->res.image["background"_H]->set_position(glm::vec2(x_pos, y_pos));
    app->res.image["background"_H]->set_size(glm::vec2(width, height));

    update_loading_animation();

    loader->update(app);
}

void LoadingScene::setup_widgets() {
    static constexpr int LOWEST_RESOLUTION = 288;
    static constexpr int HIGHEST_RESOLUTION = 1035;

    auto loading_text = app->res.text.load(
        "loading_text"_H,
        app->res.font["good_dog_plain"_H],
        "Loading",
        1.5f,
        glm::vec3(0.81f)
    );
    loading_text->stick(gui::Sticky::SE);
    loading_text->offset(20, gui::Relative::Right);
    loading_text->offset(20, gui::Relative::Bottom);
    loading_text->scale(0.4f, 1.3f, LOWEST_RESOLUTION, HIGHEST_RESOLUTION);

    const auto size = loading_text->get_actual_size();
    loading_text->fake_size(glm::vec2(size.x + 40.0f, size.y));
    loading_text->set_shadows(true);
    app->gui_renderer->add_widget(loading_text);
}

void LoadingScene::load_splash_screen_texture() {
    using namespace assets;
    using namespace encrypt;
    using namespace file_system;

    gl::TextureSpecification specification;

    app->res.texture.load("splash_screen"_H, encr(path_for_assets(SPLASH_SCREEN_TEXTURE)), specification);
}

void LoadingScene::update_loading_animation() {
    const double current_seconds = app->window->get_time();
    const double elapsed_seconds = current_seconds - loading_animation.previous_seconds;
    loading_animation.previous_seconds = current_seconds;

    loading_animation.total_time += elapsed_seconds;

    if (loading_animation.total_time > 0.4) {
        loading_animation.total_time = 0.0;

        std::string text = "Loading";
        text.append(loading_animation.dots, '.');

        app->res.text["loading_text"_H]->set_text(text);

        loading_animation.dots++;
        loading_animation.dots %= 4;
    }
}

hs LoadingScene::scene_int_to_id(int scene) {  // FIXME find a better way
    switch (scene) {
        case 0:
            return "standard_game"_H;
        case 1:
            return "jump_variant"_H;
        case 2:
            return "jump_plus_variant"_H;
        default:
            REL_CRITICAL("Invalid scene number, exiting...");
            application_exit::panic();
    }

    return {};
}
