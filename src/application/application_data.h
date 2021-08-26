#pragma once

#include <functional>

#include "application/events.h"

struct ApplicationData {
    int width, height;
    std::function<void(events::Event&)> event_function;
};
