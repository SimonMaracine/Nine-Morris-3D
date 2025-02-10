#pragma once

#include <nine_morris_3d_engine/nine_morris_3d.hpp>
#include <networking/client.hpp>

#include "options.hpp"

// State should be global as well
enum class ConnectionState {
    Disconnected,
    Connecting,
    Connected
};

struct Global : sm::GlobalData {
    // Saved/Loaded settings
    Options options;

    // Connection to server is global, but it is only used in game scenes
    networking::Client client;
    ConnectionState connection_state {ConnectionState::Disconnected};
    bool first_connect {false};  // Set to true after the very first automatic connection attempt

    float get_scale() const { return static_cast<float>(options.scale); }
};
