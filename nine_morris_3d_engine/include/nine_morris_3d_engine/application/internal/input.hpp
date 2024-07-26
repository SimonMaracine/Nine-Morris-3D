#pragma once

#include <utility>

#include "nine_morris_3d_engine/application/input_codes.hpp"

struct GLFWwindow;

namespace sm::internal {
    class Input {
    public:
        explicit Input(GLFWwindow* window_handle) noexcept
            : window_handle(window_handle) {}

        bool is_key_pressed(Key key) const noexcept;
        bool is_mouse_button_pressed(MouseButton button) const noexcept;
        std::pair<float, float> get_mouse_position() const noexcept;

        static Key key_from_code(int code) noexcept;
        static MouseButton mouse_button_from_code(int code) noexcept;
    private:
        GLFWwindow* window_handle {};
    };
}
