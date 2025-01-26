#pragma once

#include <memory>

#include <networking/client.hpp>

#include "options.hpp"

struct Global {
    // Saved/Loaded settings
    Options options;

    // Connection to server should be global
    // Need to be a smart pointer, because it's a non-copyable + non-movable type
    std::shared_ptr<networking::Client> client;

    float get_scale() const { return static_cast<float>(options.scale); }
};
