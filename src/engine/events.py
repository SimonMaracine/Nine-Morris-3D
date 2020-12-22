from abc import ABC
from copy import copy
from enum import Enum, auto

import pyglfw.libapi as GLFW

import src.engine.display as display


class EventType(Enum):
    KEY_PRESSED = auto()
    KEY_RELEASED = auto()
    MOUSE_BUTTON_PRESSED = auto()
    MOUSE_BUTTON_RELEASED = auto()
    WINDOW_CLOSED = auto()
    MOUSE_SCROLLED = auto()
    MOUSE_MOVED = auto()


_event_list = []


class Event(ABC):
    pass


class KeyPressedEvent(Event):

    def __init__(self, key: int):
        self.type = EventType.KEY_PRESSED
        self.key = key

    def __repr__(self):
        return "KEY_PRESSED " + str(self.key)


class KeyReleasedEvent(Event):

    def __init__(self, key: int):
        self.type = EventType.KEY_RELEASED
        self.key = key

    def __repr__(self):
        return "KEY_RELEASED " + str(self.key)


class MouseButtonPressedEvent(Event):

    def __init__(self, mouse_button: int):
        self.type = EventType.MOUSE_BUTTON_PRESSED
        self.mouse_button = mouse_button

    def __repr__(self):
        return "MOUSE_BUTTON_PRESSED " + str(self.mouse_button)


class MouseButtonReleasedEvent(Event):

    def __init__(self, mouse_button: int):
        self.type = EventType.MOUSE_BUTTON_RELEASED
        self.mouse_button = mouse_button

    def __repr__(self):
        return "MOUSE_BUTTON_RELEASED " + str(self.mouse_button)


class WindowClosedEvent(Event):

    def __init__(self):
        self.type = EventType.WINDOW_CLOSED

    def __repr__(self):
        return "WINDOW_CLOSED"


class MouseScrolledEvent(Event):

    def __init__(self, scroll: int):
        self.type = EventType.MOUSE_SCROLLED
        self.scroll = scroll

    def __repr__(self):
        return "MOUSE_SCROLLED " + str(self.scrool)


class MouseMovedEvent(Event):

    def __init__(self, x_pos: float, y_pos: float):
        self.type = EventType.MOUSE_MOVED
        self.x_pos = x_pos
        self.y_pos = y_pos

    def __repr__(self):
        return f"MOUSE_MOVED {self.x_pos}, {self.y_pos}"


@GLFW.GLFWwindowclosefun
def _window_closed_callback(window):
    _post_event(WindowClosedEvent())


@GLFW.GLFWkeyfun
def _key_callback(window, key, scancode, action, mods):
    if action == GLFW.GLFW_PRESS:
        _post_event(KeyPressedEvent(key))
    elif action == GLFW.GLFW_RELEASE:
        _post_event(KeyReleasedEvent(key))


@GLFW.GLFWmousebuttonfun
def _mouse_button_callback(window, button, action, mods):
    if action == GLFW.GLFW_PRESS:
        _post_event(MouseButtonPressedEvent(button))
    elif action == GLFW.GLFW_RELEASE:
        _post_event(MouseButtonReleasedEvent(button))


@GLFW.GLFWscrollfun
def _scroll_callback(window, xoffset, yoffset):
    _post_event(MouseScrolledEvent(yoffset))


@GLFW.GLFWcursorposfun
def _cursor_position_callback(window, xpos, ypos):
    _post_event(MouseMovedEvent(xpos, ypos))


def init():
    GLFW.glfwSetWindowCloseCallback(display.windowp, _window_closed_callback)
    GLFW.glfwSetKeyCallback(display.windowp, _key_callback)
    GLFW.glfwSetMouseButtonCallback(display.windowp, _mouse_button_callback)
    GLFW.glfwSetScrollCallback(display.windowp, _scroll_callback)
    GLFW.glfwSetCursorPosCallback(display.windowp, _cursor_position_callback)


def get_events() -> list:
    GLFW.glfwPollEvents()

    events = copy(_event_list)  # shallow copy here; it should work
    _event_list.clear()

    return events


def _post_event(event: Event):
    _event_list.append(event)
