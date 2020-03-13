from pyglfw.libapi import *

from engine import display

_mouse_x_pos = 0
_mouse_y_pos = 0


def get_key_pressed(key: int) -> bool:
    return glfwGetKey(display.windowp, key) == GLFW_PRESS


def get_mouse_button_pressed(button: int) -> bool:
    return glfwGetMouseButton(display.windowp, button) == GLFW_PRESS


def get_mouse_position() -> tuple:
    return glfwGetCursorPos(display.windowp)


def get_mouse_rel_x():
    global _mouse_x_pos
    current_x_pos = get_mouse_position()[0]
    rel = current_x_pos - _mouse_x_pos
    _mouse_x_pos = current_x_pos
    return rel


def get_mouse_rel_y():
    global _mouse_y_pos
    current_y_pos = get_mouse_position()[1]
    rel = current_y_pos - _mouse_y_pos
    _mouse_y_pos = current_y_pos
    return rel
