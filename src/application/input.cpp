#include <glad/glad.h>  // Include glad just to make everything compile
#include <GLFW/glfw3.h>

#include "application/input.h"

namespace input {
    static GLFWwindow* window_ptr = nullptr;

    void initialize(GLFWwindow* window) {
        window_ptr = window;
    }

    bool is_key_pressed(Key key) {
        return glfwGetKey(window_ptr, static_cast<int>(key)) == GLFW_PRESS;
    }

    bool is_mouse_button_pressed(MouseButton button) {
        return glfwGetMouseButton(window_ptr, static_cast<int>(button)) == GLFW_PRESS;
    }

    float get_mouse_x() {
        double x, y;
        glfwGetCursorPos(window_ptr, &x, &y);

        return static_cast<float>(x);
    }

    float get_mouse_y() {
        double x, y;
        glfwGetCursorPos(window_ptr, &x, &y);

        return static_cast<float>(y);
    }

    std::pair<float, float> get_mouse() {
        double x, y;
        glfwGetCursorPos(window_ptr, &x, &y);

        return std::make_pair<float, float>(static_cast<float>(x), static_cast<float>(y));
    }
}
