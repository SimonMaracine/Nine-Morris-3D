#pragma once

#include "nine_morris_3d_engine/application/events.h"

class Application;

struct ApplicationData {
    int width, height;
    std::string title;
    std::vector<std::string> authors;  // TODO use this
    unsigned int version_major;
    unsigned int version_minor;
    unsigned int version_patch;
    Application* app;
    // std::function<void(events::Event&)> event_function;
};
