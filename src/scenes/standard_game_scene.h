#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

struct StandardGameScene : public Scene {
    StandardGameScene()
        : Scene("game") {}

    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_awake() override;
    virtual void on_update() override;
    virtual void on_fixed_update() override;
    virtual void on_imgui_update() override;
};
