#include "nine_morris_3d_engine/application/internal/input.hpp"

#include <GLFW/glfw3.h>

namespace sm::internal {
    bool Input::is_key_pressed(Key key) const {
        return glfwGetKey(window_handle, static_cast<int>(key)) == GLFW_PRESS;
    }

    bool Input::is_mouse_button_pressed(MouseButton button) const {
        return glfwGetMouseButton(window_handle, static_cast<int>(button)) == GLFW_PRESS;
    }

    std::pair<float, float> Input::get_mouse_position() const {
        double x, y;
        glfwGetCursorPos(window_handle, &x, &y);

        return std::make_pair(static_cast<float>(x), static_cast<float>(y));
    }

    Key Input::key_from_code(int code) {
        return static_cast<Key>(code);
    }

    MouseButton Input::mouse_button_from_code(int code) {
        return static_cast<MouseButton>(code);
    }
}
