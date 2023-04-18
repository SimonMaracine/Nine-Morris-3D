#pragma once

#include <engine/engine_application.h>
#include <engine/engine_graphics.h>

#include "other/display_manager.h"
#include "other/data.h"

struct LauncherScene : public Scene {
    LauncherScene()
        : Scene("launcher") {}

    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_awake() override;
    virtual void on_update() override;
    virtual void on_imgui_update() override;

    void on_window_closed(const WindowClosedEvent&);

    void page_display();
    void page_graphics();

    ImGuiWindowFlags window_flags = 0;

    DisplayManager display_manager;
};
