#pragma once

#include <engine/engine_application.h>

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
    const char* scene_int_to_string(int scene);

    std::unique_ptr<assets_load::AllStartLoader> loader;

    struct {
        double previous_seconds = 0.0;
        double total_time = 0.0;
        unsigned int dots = 0;
    } loading_animation;
};
