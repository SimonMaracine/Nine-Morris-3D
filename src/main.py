from OpenGL.GL import *
from pyglfw.libapi import *

from src.renderer import set_clear_color
from src.display import create_window, clear
from src.shader import Shader
from src.model import Model
from src.errors import get_errors
from src import events


def main():
    window = create_window()
    events.init(window)
    set_clear_color(1, 0, 0)

    model = Model("data/models/dragon.obj")

    # shader = Shader("data/shaders/vert_shader.vert", "data/shaders/frag_shader.frag")
    # shader.use()

    # positions = np.array([
    #     -0.5, -0.5, 0.0,
    #     0.5, -0.5, 0.0,
    #     0.0, 0.5, 0.0,
    # ], dtype="float32")

    get_errors()

    running = True

    while running:
        for event in events.get_events():
            print(event)
            if event.type == events.WINDOW_CLOSED:
                running = False

        clear()

        # glDrawArrays(GL_TRIANGLES, 0, 3)

        get_errors()

        glfwSwapBuffers(window)

    # shader.dispose()

    # Don't make any OpenGL calls after this!
    glfwDestroyWindow(window)
    glfwTerminate()
