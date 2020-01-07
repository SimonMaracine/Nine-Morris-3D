import pygame
from OpenGL.GL import *


class Texture:

    def __init__(self, file_path: str):
        self.width = 0
        self.height = 0
        self.data = self._load(file_path, "RGB")

        self.id = glGenTextures(1)
        glBindTexture(self.id)

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST)

        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, self.width, self.height)
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, self.width, self.height, GL_RGB, GL_UNSIGNED_BYTE, self.data)

    def bind(self):
        glBindTexture(GL_TEXTURE_2D, self.id)

    def unbind(self):
        glBindTexture(GL_TEXTURE_2D, 0)

    def dispose(self):
        glDeleteTextures(self.id)

    def _load(self, file_path: str, format: str) -> bytes:
        image_surf = pygame.image.load(file_path)
        self.width = image_surf.get_width()
        self.height = image_surf.get_height()

        return pygame.image.tostring(image_surf, format, False)
