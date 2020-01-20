from OpenGL.GL import *
from pyglfw.libapi import *

WIDTH = 1280
HEIGHT = 720

windowp = None


@GLFWframebuffersizefun
def _framebuffer_size_callback(window, width, height):  # TODO maybe not necessary
    global WIDTH, HEIGHT
    glViewport(0, 0, width, height)
    WIDTH = width
    HEIGHT = height


def init():
    if not glfwInit():
        raise RuntimeError("Could not initialise GLFW")


def create_window():
    global windowp
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3)
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3)
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE)

    glfwWindowHint(GLFW_RESIZABLE, False)  # weird...

    windowp = glfwCreateWindow(WIDTH, HEIGHT, b"Nine Norris 3D", None, None)

    glfwMakeContextCurrent(windowp)
    glfwSwapInterval(1)

    glfwSetFramebufferSizeCallback(windowp, _framebuffer_size_callback)


def update():
    glfwSwapBuffers(windowp)


def dispose():
    # Don't make any OpenGL calls after this!
    glfwDestroyWindow(windowp)
    glfwTerminate()
