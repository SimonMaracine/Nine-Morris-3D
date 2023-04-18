#pragma once

#include <engine/public/application_base.h>

class DisplayManager {
public:
    DisplayManager() = default;
    DisplayManager(Ctx* ctx);
    ~DisplayManager() = default;

    const std::vector<Monitor>& get_monitors() { return monitors; }
    std::vector<const char*> get_resolutions();
    std::vector<const char*> get_monitor_names();
private:
    std::vector<Monitor> monitors;
    size_t resolutions_supported = 0;

    Ctx* ctx = nullptr;
};
