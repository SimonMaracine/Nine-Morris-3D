#pragma once

#include <functional>
#include <string>

#include "application/events.h"

struct ApplicationData {
    int width, height;
    std::string title;
    std::function<void(events::Event&)> event_function;
};
