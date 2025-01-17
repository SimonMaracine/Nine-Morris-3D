#pragma once

#include "options.hpp"

struct Global {
    Options options;

    float get_scale() const { return static_cast<float>(options.scale); }
};
