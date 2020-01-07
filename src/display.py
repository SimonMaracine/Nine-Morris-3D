from OpenGL.GL import *
from pyglfw.libapi import *

WIDTH = 1280
HEIGHT = 720

if not glfwInit():
    raise RuntimeError("Could not initialise GLFW")


def create_window():
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE)

    window = glfwCreateWindow(WIDTH, HEIGHT, b"Nine Norris 3D", None, None)

    glfwMakeContextCurrent(window)
    glfwSwapInterval(1)

    return window


def set_clear_color(red: float, green: float, blue: float):
    glClearColor(red, green, blue, 1)


def clear():
    glClear(GL_COLOR_BUFFER_BIT)
