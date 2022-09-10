#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

struct LoadingScene : public Scene {
    LoadingScene()
        : Scene("loading") {}

    virtual void on_start(Application* app) override;
    virtual void on_stop(Application* app) override;
    virtual void on_update(Application* app) override;
};
