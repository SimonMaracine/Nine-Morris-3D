#pragma once

#include <utility>

#include "nine_morris_3d_engine/application/input_codes.hpp"

struct GLFWwindow;

namespace sm::internal {
    // Input polling API
    class Input {
    public:
        explicit Input(GLFWwindow* window_handle) noexcept
            : m_window_handle(window_handle) {}

        // Get the current state of a key
        bool is_key_pressed(Key key) const noexcept;

        // Get the current state of a mouse button
        bool is_button_pressed(Button button) const noexcept;

        // Get the current state of the mouse
        std::pair<float, float> get_mouse_position() const noexcept;

        // Conversions
        // static Key key_from_code(int code) noexcept;
        // static MouseButton mouse_button_from_code(int code) noexcept;
    private:
        GLFWwindow* m_window_handle {};
    };
}
