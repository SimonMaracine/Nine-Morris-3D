#pragma once

#include <nine_morris_3d_engine/engine_application.h>

class DisplayManager {
public:
    DisplayManager() = default;
    DisplayManager(Application* app);
    ~DisplayManager() = default;

    const std::vector<Monitor>& get_monitors() { return monitors; }
    std::vector<const char*> get_resolutions();
    std::vector<const char*> get_monitor_names();
private:
    std::vector<Monitor> monitors;
    size_t resolutions_supported = 0;

    Application* app = nullptr;
};
