#include <glad/glad.h>  // Include glad just to make everything compile
#include <GLFW/glfw3.h>

#include "engine/application/input.h"

namespace input {
    static GLFWwindow* _window_handle = nullptr;

    void initialize(GLFWwindow* window_handle) {
        _window_handle = window_handle;
    }

    bool is_key_pressed(Key key) {
        return glfwGetKey(_window_handle, static_cast<int>(key)) == GLFW_PRESS;
    }

    bool is_mouse_button_pressed(MouseButton button) {
        return glfwGetMouseButton(_window_handle, static_cast<int>(button)) == GLFW_PRESS;
    }

    float get_mouse_x() {
        double x, y;
        glfwGetCursorPos(_window_handle, &x, &y);

        return static_cast<float>(x);
    }

    float get_mouse_y() {
        double x, y;
        glfwGetCursorPos(_window_handle, &x, &y);

        return static_cast<float>(y);
    }

    std::pair<float, float> get_mouse() {
        double x, y;
        glfwGetCursorPos(_window_handle, &x, &y);

        return std::make_pair(static_cast<float>(x), static_cast<float>(y));
    }

    Key key_from_code(int code) {
        return static_cast<Key>(code);
    }

    MouseButton mouse_button_from_code(int code) {
        return static_cast<MouseButton>(code);
    }
}
