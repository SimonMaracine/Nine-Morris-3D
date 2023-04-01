#include <engine/engine_application.h>
#include <engine/engine_other.h>

#include "other/display_manager.h"

static const char* RESOLUTIONS[] = {
    "512x288", "768x432", "1024x576", "1280x720", "1536x864", "1792x1008"
};

DisplayManager::DisplayManager(Ctx* ctx)
    : ctx(ctx) {
    monitors = ctx->window->get_monitors();

    const auto resolution = monitors[0].get_resolution();
    const int width = resolution.first;

    if (width < 512) {
        LOG_DIST_CRITICAL("Monitor has unsupported resolution, exiting...");
        panic::panic();
    } else if (width < 768) {
        resolutions_supported = 1;
    } else if (width < 1024) {
        resolutions_supported = 2;
    } else if (width < 1280) {
        resolutions_supported = 3;
    } else if (width < 1536) {
        resolutions_supported = 4;
    } else if (width < 1792) {
        resolutions_supported = 5;
    } else {
        resolutions_supported = 6;
    }
}

std::vector<const char*> DisplayManager::get_resolutions() {
    std::vector<const char*> resolutions;

    for (size_t i = 0; i < resolutions_supported; i++) {
        resolutions.push_back(RESOLUTIONS[i]);
    }

    return resolutions;
}

std::vector<const char*> DisplayManager::get_monitor_names() {
    std::vector<const char*> monitor_names;

    for (Monitor monitor : monitors) {
        monitor_names.push_back(monitor.get_name());
    }

    return monitor_names;
}
