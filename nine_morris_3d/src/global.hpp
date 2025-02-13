#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>

#include "options.hpp"
#include "client.hpp"

struct Global : sm::GlobalData {
    // Saved/Loaded settings
    Options options;

    // Connection to server is global, but it is only used in game scenes
    Client client;

    float get_scale() const { return static_cast<float>(options.scale); }
};
