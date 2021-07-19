#pragma once

#include <GLFW/glfw3.h>

namespace input {
    void init(GLFWwindow* window);
    bool is_key_pressed(int key);
    bool is_mouse_button_pressed(int button);
    int get_mouse_x();
    int get_mouse_y();
}
