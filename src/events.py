from abc import ABC
from copy import copy

from pyglfw.libapi import *

KEY_PRESSED = 1
KEY_RELEASED = 2
MOUSE_BUTTON_PRESSED = 3
MOUSE_BUTTON_RELEASED = 4
WINDOW_CLOSED = 5
MOUSE_SCROLLED = 6
MOUSE_MOVED = 7

_event_list = []


class Event(ABC):
    pass


class KeyPressedEvent(Event):

    def __init__(self, key: int):
        self.type = KEY_PRESSED
        self.key = key

    def __repr__(self):
        return "KEY_PRESSED " + str(self.key)


class KeyReleasedEvent(Event):

    def __init__(self, key: int):
        self.type = KEY_RELEASED
        self.key = key

    def __repr__(self):
        return "KEY_RELEASED " + str(self.key)


class MouseButtonPressedEvent(Event):

    def __init__(self, mouse_button: int):
        self.type = MOUSE_BUTTON_PRESSED
        self.mouse_button = mouse_button

    def __repr__(self):
        return "MOUSE_BUTTON_PRESSED " + str(self.mouse_button)


class MouseButtonReleasedEvent(Event):

    def __init__(self, mouse_button: int):
        self.type = MOUSE_BUTTON_RELEASED
        self.mouse_button = mouse_button

    def __repr__(self):
        return "MOUSE_BUTTON_RELEASED " + str(self.mouse_button)


class WindowClosedEvent(Event):

    def __init__(self):
        self.type = WINDOW_CLOSED

    def __repr__(self):
        return "WINDOW_CLOSED"


class MouseScrolledEvent(Event):

    def __init__(self, scrool: int):
        self.type = MOUSE_SCROLLED
        self.scrool = scrool

    def __repr__(self):
        return "MOUSE_SCROLLED " + str(self.scrool)


class MouseMovedEvent(Event):

    def __init__(self, x_pos: float, y_pos: float):
        self.type = MOUSE_MOVED
        self.x_pos = x_pos
        self.y_pos = y_pos

    def __repr__(self):
        return f"MOUSE_SCROLLED {self.x_pos}, {self.y_pos}"


@GLFWwindowclosefun
def _window_closed_callback(window):
    _post_event(WindowClosedEvent())


@GLFWkeyfun
def _key_callback(window, key, scancode, action, mods):
    if action == GLFW_PRESS:
        _post_event(KeyPressedEvent(key))
    elif action == GLFW_RELEASE:
        _post_event(KeyReleasedEvent(key))


@GLFWmousebuttonfun
def _mouse_button_callback(window, button, action, mods):
    if action == GLFW_PRESS:
        _post_event(MouseButtonPressedEvent(button))
    elif action == GLFW_RELEASE:
        _post_event(MouseButtonReleasedEvent(button))


@GLFWscrollfun
def _scrool_callback(window, xoffset, yoffset):
    _post_event(MouseScrolledEvent(yoffset))


@GLFWcursorposfun
def _cursor_position_callback(window, xpos, ypos):
    _post_event(MouseMovedEvent(xpos, ypos))


def init(window):
    glfwSetWindowCloseCallback(window, _window_closed_callback)
    glfwSetKeyCallback(window, _key_callback)
    glfwSetMouseButtonCallback(window, _mouse_button_callback)
    glfwSetScrollCallback(window, _scrool_callback)
    glfwSetCursorPosCallback(window, _cursor_position_callback)


def get_events() -> list:
    glfwPollEvents()

    events = copy(_event_list)  # shallow copy here; it should work
    _event_list.clear()

    return events


def _post_event(event: Event):
    _event_list.append(event)
