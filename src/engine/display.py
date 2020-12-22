import OpenGL.GL as GL
import pyglfw.libapi as GLFW

WIDTH = 1280
HEIGHT = 720

windowp = None


@GLFW.GLFWframebuffersizefun
def _framebuffer_size_callback(window, width, height):  # TODO maybe not necessary
    global WIDTH, HEIGHT
    GL.glViewport(0, 0, width, height)
    WIDTH = width
    HEIGHT = height


def init():
    global windowp
    if not GLFW.glfwInit():
        raise RuntimeError("Could not initialise GLFW")

    GLFW.glfwWindowHint(GLFW.GLFW_CONTEXT_VERSION_MAJOR, 3)
    GLFW.glfwWindowHint(GLFW.GLFW_CONTEXT_VERSION_MINOR, 3)
    GLFW.glfwWindowHint(GLFW.GLFW_OPENGL_PROFILE, GLFW.GLFW_OPENGL_CORE_PROFILE)
    GLFW.glfwWindowHint(GLFW.GLFW_RESIZABLE, False)  # weird...

    windowp = GLFW.glfwCreateWindow(WIDTH, HEIGHT, b"Nine Norris 3D", None, None)

    GLFW.glfwMakeContextCurrent(windowp)
    GLFW.glfwSwapInterval(1)

    GLFW.glfwSetFramebufferSizeCallback(windowp, _framebuffer_size_callback)


def update():
    # Must be called after all drawing
    GLFW.glfwSwapBuffers(windowp)


def dispose():
    # Don't make any OpenGL calls after this!
    GLFW.glfwDestroyWindow(windowp)
    GLFW.glfwTerminate()
