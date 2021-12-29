#include <utility>

#include <GLFW/glfw3.h>

namespace input {
    static GLFWwindow* window_ptr = nullptr;

    void initialize(GLFWwindow* window) {
        window_ptr = window;
    }

    bool is_key_pressed(int key) {
        return glfwGetKey(window_ptr, key) == GLFW_PRESS;
    }

    bool is_mouse_button_pressed(int button) {
        return glfwGetMouseButton(window_ptr, button) == GLFW_PRESS;
    }

    float get_mouse_x() {
        double x, y;
        glfwGetCursorPos(window_ptr, &x, &y);

        return (float) x;
    }

    float get_mouse_y() {
        double x, y;
        glfwGetCursorPos(window_ptr, &x, &y);

        return (float) y;
    }

    const std::pair<float, float> get_mouse() {
        double x, y;
        glfwGetCursorPos(window_ptr, &x, &y);

        return std::make_pair<float, float>(x, y);
    }
}
