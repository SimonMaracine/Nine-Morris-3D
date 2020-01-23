import numpy as np
from OpenGL.GL import *

from src.disposable import Disposable


class VertexArray(Disposable):

    def __init__(self):
        super().__init__()

        self.id = glGenVertexArrays(1)
        glBindVertexArray(self.id)

        self.vertex_count = 0
        self.index_buffer = 0

    def add_attribute(self, attr_list: int, size: int, data: list):
        buffer = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, buffer)
        glBufferData(GL_ARRAY_BUFFER, np.array(data, dtype="float32"), GL_STATIC_DRAW)

        glVertexAttribPointer(attr_list, size, GL_FLOAT, GL_FALSE, 0, None)
        glEnableVertexAttribArray(attr_list)

    def add_index_buffer(self, data: list):
        self.index_buffer = glGenBuffers(1)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.index_buffer)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, np.array(data, dtype="uint32"), GL_STATIC_DRAW)

        self.vertex_count = len(data)

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)

    def bind(self):
        glBindVertexArray(self.id)

    def unbind(self):
        glBindVertexArray(0)

    def dispose(self):
        super().dispose()
        glDeleteVertexArrays(1, self.id)

    def enable_index_buffer(self):
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self.index_buffer)
