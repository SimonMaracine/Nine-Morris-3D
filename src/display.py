from typing import Optional

import pygame
from OpenGL.GL import *

WIDTH = 1280
HEIGHT = 720

window: Optional[pygame.Surface] = None
clock = pygame.time.Clock()

pygame.init()


def create_window():
    global window
    pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MAJOR_VERSION, 3)
    pygame.display.gl_set_attribute(pygame.GL_CONTEXT_MINOR_VERSION, 3)
    pygame.display.gl_set_attribute(pygame.GL_CONTEXT_PROFILE_MASK, pygame.GL_CONTEXT_PROFILE_CORE)
    window = pygame.display.set_mode((WIDTH, HEIGHT), pygame.DOUBLEBUF | pygame.HWSURFACE | pygame.OPENGL)
    # glViewport(0, 0, WIDTH, HEIGHT)


def set_clear_color(red: float, green: float, blue: float):
    glClearColor(red, green, blue, 1)


def clear():
    glClear(GL_COLOR_BUFFER_BIT)
