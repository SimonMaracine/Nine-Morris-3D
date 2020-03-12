import glm
import numpy as np
from OpenGL.GL import *

from src.log import get_logger

logger = get_logger(__name__)
logger.setLevel(10)


class Shader:

    def __init__(self, vert_file: str, frag_file: str):
        self.id = glCreateProgram()
        logger.debug(f"Shader program id {self.id}")

        self.vertex_shader_id = Shader._compile_shader(vert_file, GL_VERTEX_SHADER)
        self.fragment_shader_id = Shader._compile_shader(frag_file, GL_FRAGMENT_SHADER)

        glAttachShader(self.id, self.vertex_shader_id)
        glAttachShader(self.id, self.fragment_shader_id)
        glLinkProgram(self.id)
        glValidateProgram(self.id)

        status = glGetProgramiv(self.id, GL_LINK_STATUS)

        if status == GL_FALSE:
            logger.critical("Shader program linking failed")
            print(glGetShaderInfoLog(self.id))
        else:
            logger.info("Shader program linked successfully")

    def use(self):
        glUseProgram(self.id)

    def stop(self):
        glUseProgram(0)

    def dispose(self):
        glDetachShader(self.id, self.vertex_shader_id)
        glDetachShader(self.id, self.fragment_shader_id)
        glDeleteShader(self.vertex_shader_id)
        glDeleteShader(self.fragment_shader_id)
        glDeleteProgram(self.id)

    def upload_uniform_float3(self, name: str, vector: glm.vec3):
        loc = self._get_uniform_location(name)
        glUniform3f(loc, vector.x, vector.y, vector.z)

    def upload_uniform_float2(self, name: str, vector: glm.vec2):
        loc = self._get_uniform_location(name)
        glUniform2f(loc, vector.x, vector.y)

    def upload_uniform_int1(self, name: str, value: int):
        loc = self._get_uniform_location(name)
        glUniform1i(loc, value)

    def upload_uniform_float16(self, name: str, matrix: glm.mat4):
        loc = self._get_uniform_location(name)
        glUniformMatrix4fv(loc, 1, GL_FALSE, np.array(matrix, dtype="float32"))  # TODO maybe it works?

    def _get_uniform_location(self, name: str) -> int:
        location = glGetUniformLocation(self.id, name)
        if location == -1:
            logger.error(f"Uniform variable '{name}' not found")

        return location

    @staticmethod
    def _compile_shader(source_file: str, type: int) -> int:
        with open(source_file, "r") as file:
            contents: str = file.read()

        id = glCreateShader(type)
        glShaderSource(id, contents)
        glCompileShader(id)

        status = glGetShaderiv(id, GL_COMPILE_STATUS)

        if status == GL_FALSE:
            logger.critical("Shader compilation failed")
            print(glGetShaderInfoLog(id))
        else:
            logger.info("Shader compiled successfully")

        return id
