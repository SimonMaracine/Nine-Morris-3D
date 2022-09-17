#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

struct LauncherScene : public Scene {
    LauncherScene()
        : Scene("launcher_scene") {}
    virtual ~LauncherScene() = default;

    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_imgui_update() override;

    void on_window_closed(const WindowClosedEvent& event);
};
