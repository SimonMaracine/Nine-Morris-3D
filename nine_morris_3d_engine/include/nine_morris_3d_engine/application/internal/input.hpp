#pragma once

#include <utility>

#include "nine_morris_3d_engine/application/input_codes.hpp"

// Input polling API

namespace sm::internal {
    // Get the current state of a key
    bool is_key_pressed(Key key) noexcept;

    // Get the current state of a mouse button
    bool is_button_pressed(Button button) noexcept;

    // Get the current state of the mouse
    std::pair<float, float> get_mouse_position() noexcept;

    // Conversions
    // static Key key_from_code(int code) noexcept;
    // static MouseButton mouse_button_from_code(int code) noexcept;
}
