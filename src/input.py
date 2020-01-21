from pyglfw.libapi import *

import src.display as display


def get_key_pressed(key: int) -> bool:
    return glfwGetKey(display.windowp, key) == GLFW_PRESS


def get_mouse_button_pressed(button: int) -> bool:
    return glfwGetMouseButton(display.windowp, button) == GLFW_PRESS


def get_mouse_poition() -> tuple:
    return glfwGetCursorPos(display.windowp)
