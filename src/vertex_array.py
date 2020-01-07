import numpy as np
from OpenGL.GL import *


class VertexArray:

    def __init__(self):
        self.id = glGenVertexArrays(1)
        glBindVertexArray(self.id)

        self.attributes = []
        self.vertex_count = 0

    def add_attribute(self, attr_list: int, size: int, data: list):
        buffer = glGenBuffers(1)
        glBindBuffer(GL_ARRAY_BUFFER, buffer)
        glBufferData(GL_ARRAY_BUFFER, np.array(data, dtype="float32"), GL_STATIC_DRAW)

        glVertexAttribPointer(attr_list, size, GL_FLOAT, GL_FALSE, 0, None)
        self.attributes.append(attr_list)

    def add_index_buffer(self, data: list):
        buffer = glGenBuffers(1)
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer)
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, np.array(data, dtype="int32"), GL_STATIC_DRAW)

        self.vertex_count = len(data)

    def dispose(self):
        glDeleteVertexArrays(self.id)

    def enable_attributes(self):
        for attr in self.attributes:
            glEnableVertexAttribArray(attr)
