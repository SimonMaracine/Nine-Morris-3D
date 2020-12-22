import pygame
import OpenGL.GL as GL


class Texture:

    def __init__(self, file_path: str, has_alpha: bool = False):
        self.width = 0
        self.height = 0

        if not has_alpha:
            tex_format = "RGB"
        else:
            tex_format = "RGBA"

        self.data = self._load(file_path, tex_format)

        self.id = GL.glGenTextures(1)
        GL.glBindTexture(GL.GL_TEXTURE_2D, self.id)

        GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_S, GL.GL_REPEAT)
        GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_WRAP_T, GL.GL_REPEAT)
        GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MIN_FILTER, GL.GL_NEAREST)
        GL.glTexParameteri(GL.GL_TEXTURE_2D, GL.GL_TEXTURE_MAG_FILTER, GL.GL_NEAREST)

        GL.glTexStorage2D(GL.GL_TEXTURE_2D, 1, GL.GL_RGB8, self.width, self.height)
        GL.glTexSubImage2D(GL.GL_TEXTURE_2D, 0, 0, 0, self.width, self.height, GL.GL_RGB, GL.GL_UNSIGNED_BYTE, self.data)

    def bind(self, unit: int):
        GL.glActiveTexture(GL.GL_TEXTURE0 + unit)
        GL.glBindTexture(GL.GL_TEXTURE_2D, self.id)

    def unbind(self):
        GL.glBindTexture(GL.GL_TEXTURE_2D, 0)

    def dispose(self):
        GL.glDeleteTextures(self.id)

    def _load(self, file_path: str, format: str) -> bytes:
        image_surf = pygame.image.load(file_path)
        self.width = image_surf.get_width()
        self.height = image_surf.get_height()

        return pygame.image.tostring(image_surf, format, False)
