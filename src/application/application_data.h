#pragma once

#include "application/events.h"

struct ApplicationData {
    int width = 0, height = 0;
    std::string title;
    std::function<void(events::Event&)> event_function;
};
