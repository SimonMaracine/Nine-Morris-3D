import numpy as np
from OpenGL.GL import *
from pyglfw.libapi import *

from src.display import create_window, set_clear_color, clear
from src.shader import Shader
from src.errors import get_errors


def main():
    window = create_window()
    set_clear_color(1, 0, 0)

    shader = Shader("data/shaders/vert_shader.vert", "data/shaders/frag_shader.frag")
    shader.use()

    positions = np.array([
        -0.5, -0.5, 0.0,
        0.5, -0.5, 0.0,
        0.0, 0.5, 0.0,
        # -0.5, 0.5, 0.0,
        # -0.5, -0.5, 0.0,
        # 0.5, 0.5, 0.0,
        # 0.5, 0.5, 0.0,
        # -0.5, -0.5, 0.0,
        # 0.5, -0.5, 0.0
    ], dtype="float32")

    vertex_array = glGenVertexArrays(1)
    glBindVertexArray(vertex_array)

    buffer = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, buffer)
    glBufferData(GL_ARRAY_BUFFER, positions, GL_STATIC_DRAW)

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, None)

    get_errors()

    while not glfwWindowShouldClose(window):
        # for event in pygame.event.get():
        #     if event.type == pygame.QUIT:
        #         running = False
        glfwPollEvents()

        clear()

        glDrawArrays(GL_TRIANGLES, 0, 3)

        get_errors()

        glfwSwapBuffers(window)

    shader.dispose()

    # Don't make any OpenGL calls after this!
    glfwDestroyWindow(window)
    glfwTerminate()
