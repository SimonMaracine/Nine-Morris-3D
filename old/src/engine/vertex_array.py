import numpy as np
import OpenGL.GL as GL


class VertexArray:

    def __init__(self):
        self.id = GL.glGenVertexArrays(1)
        GL.glBindVertexArray(self.id)

        self.buffers = []
        self.vertex_count = 0
        self.index_buffer = 0

    def add_attribute(self, attr_list: int, size: int, data: list):
        buffer = GL.glGenBuffers(1)
        GL.glBindBuffer(GL.GL_ARRAY_BUFFER, buffer)
        GL.glBufferData(GL.GL_ARRAY_BUFFER, np.array(data, dtype="float32"), GL.GL_STATIC_DRAW)

        GL.glVertexAttribPointer(attr_list, size, GL.GL_FLOAT, GL.GL_FALSE, 0, None)
        GL.glEnableVertexAttribArray(attr_list)

        self.buffers.append(buffer)

    def add_index_buffer(self, data: list):
        self.index_buffer = GL.glGenBuffers(1)
        GL.glBindBuffer(GL.GL_ELEMENT_ARRAY_BUFFER, self.index_buffer)
        GL.glBufferData(GL.GL_ELEMENT_ARRAY_BUFFER, np.array(data, dtype="uint32"), GL.GL_STATIC_DRAW)

        self.vertex_count = len(data)

        GL.glBindBuffer(GL.GL_ELEMENT_ARRAY_BUFFER, 0)

    def bind(self):
        GL.glBindVertexArray(self.id)

    def unbind(self):
        GL.glBindVertexArray(0)

    def dispose(self):
        GL.glDeleteVertexArrays(1, self.id)
        for buffer in self.buffers:
            GL.glDeleteBuffers(1, buffer)

    def enable_index_buffer(self):
        GL.glBindBuffer(GL.GL_ELEMENT_ARRAY_BUFFER, self.index_buffer)
