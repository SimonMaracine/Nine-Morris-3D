import glm
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
        glUniform3f(name, vector.x, vector.y, vector.z)

    def upload_uniform_float2(self, name: str, vector: glm.vec2):
        glUniform2f(name, vector.x, vector.y)

    def upload_uniform_int1(self, name: str, value: int):
        glUniform1i(name, value)

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
