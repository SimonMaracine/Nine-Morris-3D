#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

#include "game/assets_load.h"
#include "launcher/launcher_options.h"

struct LoadingScene : public Scene {
    LoadingScene()
        : Scene("loading") {}

    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_update() override;

    void load_splash_screen_texture();
    void update_loading_animation();

    std::unique_ptr<assets_load::CustomLoader> loader;

    struct {
        double previous_seconds = 0.0;
        double total_time = 0.0;
        unsigned int dots = 0;
    } loading_animation;
};
