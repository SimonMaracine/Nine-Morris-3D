#pragma once

#include <utility>

#include "nine_morris_3d_engine/application/internal/input_codes.hpp"

// Input polling API

namespace sm::internal {
    // Get the current state of a key
    bool is_key_pressed(Key key);

    // Get the current state of a mouse button
    bool is_button_pressed(Button button);

    // Get the current state of the mouse
    std::pair<float, float> get_mouse_position();
}
