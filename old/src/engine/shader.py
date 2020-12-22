import glm
import numpy as np
import OpenGL.GL as GL

from src.engine.log import get_logger

logger = get_logger(__name__)
logger.setLevel(10)


class Shader:

    def __init__(self, vert_file: str, frag_file: str):
        self.id = GL.glCreateProgram()
        logger.debug(f"Shader program id {self.id}")

        self.vertex_shader_id = Shader._compile_shader(vert_file, GL.GL_VERTEX_SHADER)
        self.fragment_shader_id = Shader._compile_shader(frag_file, GL.GL_FRAGMENT_SHADER)

        GL.glAttachShader(self.id, self.vertex_shader_id)
        GL.glAttachShader(self.id, self.fragment_shader_id)
        GL.glLinkProgram(self.id)
        GL.glValidateProgram(self.id)

        status = GL.glGetProgramiv(self.id, GL.GL_LINK_STATUS)

        if status == GL.GL_FALSE:
            logger.critical("Shader program linking failed")
            print(GL.glGetShaderInfoLog(self.id))
        else:
            logger.info("Shader program linked successfully")

    def use(self):
        GL.glUseProgram(self.id)

    def stop(self):
        GL.glUseProgram(0)

    def dispose(self):
        GL.glDetachShader(self.id, self.vertex_shader_id)
        GL.glDetachShader(self.id, self.fragment_shader_id)
        GL.glDeleteShader(self.vertex_shader_id)
        GL.glDeleteShader(self.fragment_shader_id)
        GL.glDeleteProgram(self.id)

    def upload_uniform_float3(self, name: str, vector: glm.vec3):
        loc = self._get_uniform_location(name)
        GL.glUniform3f(loc, vector.x, vector.y, vector.z)

    def upload_uniform_float2(self, name: str, vector: glm.vec2):
        loc = self._get_uniform_location(name)
        GL.glUniform2f(loc, vector.x, vector.y)

    def upload_uniform_int1(self, name: str, value: int):
        loc = self._get_uniform_location(name)
        GL.glUniform1i(loc, value)

    def upload_uniform_float16(self, name: str, matrix: glm.mat4):
        loc = self._get_uniform_location(name)
        GL.glUniformMatrix4fv(loc, 1, GL.GL_FALSE, np.array(matrix, dtype="float32"))  # TODO maybe it works?

    def _get_uniform_location(self, name: str) -> int:
        location = GL.glGetUniformLocation(self.id, name)
        if location == -1:
            logger.error(f"Uniform variable '{name}' not found")

        return location

    @staticmethod
    def _compile_shader(source_file: str, type: int) -> int:
        with open(source_file, "r") as file:
            contents: str = file.read()

        id = GL.glCreateShader(type)
        GL.glShaderSource(id, contents)
        GL.glCompileShader(id)

        status = GL.glGetShaderiv(id, GL.GL_COMPILE_STATUS)

        if status == GL.GL_FALSE:
            logger.critical("Shader compilation failed")
            print(GL.glGetShaderInfoLog(id))
        else:
            logger.info("Shader compiled successfully")

        return id
