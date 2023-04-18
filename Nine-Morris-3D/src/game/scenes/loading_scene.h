#pragma once

#include <engine/public/application_base.h>

#include "game/assets_load.h"
#include "launcher/launcher_options.h"

struct LoadingScene : public Scene {
    LoadingScene()
        : Scene("loading") {}

    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_update() override;

    void setup_widgets();
    void load_splash_screen_texture();
    void update_loading_animation();
    hs scene_int_to_id(int scene);

    void initialize_board();
    void initialize_board_paint();
    void initialize_pieces();
    void initialize_node(size_t index);
    void initialize_nodes();
    void initialize_board_no_normal();
    void initialize_board_paint_no_normal();
    void initialize_pieces_no_normal();
    void initialize_skybox();
    void initialize_indicators();
    void change_board_paint_texture();
    void initialize_ids();

    void initialize_keyboard_controls();
    void initialize_light_bulb();
    void initialize_light();
    void initialize_game();  // Must be called only once

    std::unique_ptr<assets_load::AllStartLoader> loader;

    struct {
        double previous_seconds = 0.0;
        double total_time = 0.0;
        unsigned int dots = 0;
    } loading_animation;
};
