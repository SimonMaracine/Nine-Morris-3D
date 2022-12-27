#pragma once

#include "engine/application/events.h"

class Application;

struct ApplicationData {
    int width;
    int height;
    std::string title;
    bool fullscreen;
    bool native_resolution;
    bool resizable;
    int min_width;
    int min_height;
    std::string app_name;
    unsigned int version_major;
    unsigned int version_minor;
    unsigned int version_patch;
    std::vector<std::string> authors;

    Application* app = nullptr;
};
