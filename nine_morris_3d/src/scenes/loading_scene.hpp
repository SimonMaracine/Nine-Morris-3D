#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

struct LoadingScene : sm::ApplicationScene {
    explicit LoadingScene(sm::Ctx& ctx)
        : sm::ApplicationScene(ctx) {}

    SM_SCENE_NAME("loading")

    void on_start() override;
    void on_stop() override;
    void on_update() override;

    void load_assets();

    bool done {false};
};
