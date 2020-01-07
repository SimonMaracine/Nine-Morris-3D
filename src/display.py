from OpenGL.GL import *
from pyglfw.libapi import *

WIDTH = 1280
HEIGHT = 720


@GLFWframebuffersizefun
def _framebuffer_size_callback(window, width, height):  # TODO maybe not necessary
    global WIDTH, HEIGHT
    glViewport(0, 0, width, height)
    WIDTH = width
    HEIGHT = height


if not glfwInit():
    raise RuntimeError("Could not initialise GLFW")


def create_window():
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE)

    glfwWindowHint(GLFW_RESIZABLE, False)  # weird...

    window = glfwCreateWindow(WIDTH, HEIGHT, b"Nine Norris 3D", None, None)

    glfwMakeContextCurrent(window)
    glfwSwapInterval(1)

    glfwSetFramebufferSizeCallback(window, _framebuffer_size_callback)

    return window


def clear():
    glClear(GL_COLOR_BUFFER_BIT)
