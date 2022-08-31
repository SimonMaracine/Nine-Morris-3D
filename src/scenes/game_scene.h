#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

struct GameScene : public Scene {
    GameScene()
        : Scene("game") {}

    virtual void on_start(Application* app) override;
    virtual void on_stop(Application* app) override;
    virtual void on_awake(Application* app) override;
    virtual void on_update(Application* app) override;
    virtual void on_fixed_update(Application* app) override;
    virtual void on_imgui_update(Application* app) override;
    // virtual void on_event(Application* app, events::Event& event) override;
};
