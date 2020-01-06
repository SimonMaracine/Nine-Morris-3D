import pygame
import numpy as np
from OpenGL.GL import *

from src.display import create_window, set_clear_color, clock, clear
from src.shader import Shader
from src.errors import get_errors


def main():
    create_window()
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

    running = True

    vertex_array = glGenVertexArrays(1)
    glBindVertexArray(vertex_array)

    buffer = glGenBuffers(1)
    glBindBuffer(GL_ARRAY_BUFFER, buffer)
    glBufferData(GL_ARRAY_BUFFER, positions, GL_STATIC_DRAW)

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0)

    get_errors()

    while running:
        for event in pygame.event.get():
            if event.type == pygame.QUIT:
                running = False

        clear()

        glDrawArrays(GL_TRIANGLES, 0, 3)

        get_errors()

        pygame.display.flip()
        clock.tick(60)

    shader.dispose()

    # Don't make any OpenGL calls after this!
    pygame.quit()
