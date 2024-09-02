#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "options.hpp"

struct Global {
    Options options;

    float get_scale() const { return static_cast<float>(options.scale); }
};
