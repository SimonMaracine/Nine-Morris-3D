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

    // Connection to server should be global
    networking::Client client;
    ConnectionState connection_state {ConnectionState::Disconnected};

    float get_scale() const { return static_cast<float>(options.scale); }
};
