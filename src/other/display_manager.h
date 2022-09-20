#pragma once

#include <nine_morris_3d_engine/nine_morris_3d_engine.h>

class DisplayManager {
public:
    DisplayManager() = default;
    DisplayManager(Application* app);
    ~DisplayManager() = default;

    const std::vector<Monitor>& get_monitors() { return monitors; }
    std::vector<const char*> get_resolutions();
    std::vector<const char*> get_monitor_names();
private:
    Application* app = nullptr;

    std::vector<Monitor> monitors;
    size_t resolutions_supported = 0;
};
