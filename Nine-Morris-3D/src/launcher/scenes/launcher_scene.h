#pragma once

#include <engine/public/application_base.h>
#include <engine/public/graphics.h>

#include "other/data.h"

struct LauncherScene : public sm::Scene {
    LauncherScene()
        : Scene("launcher") {}

    virtual void on_start() override;
    virtual void on_stop() override;
    virtual void on_awake() override;
    virtual void on_update() override;
    virtual void on_imgui_update() override;

    void on_window_closed(const sm::WindowClosedEvent&);

    void page_display();
    void page_graphics();

    void initialize_resolutions();
    std::vector<const char*> get_available_resolutions();

    ImGuiWindowFlags window_flags = 0;

    size_t resolutions_supported = 0;
};
