from typing import Optional

import glm
import OpenGL
OpenGL.ERROR_CHECKING = False
from OpenGL.GL import *

import src.display as display
from src.entity import Entity
from src.shader import Shader
from src.camera import Camera

_basic_shader: Optional[Shader] = None


def init():
    global _basic_shader
    _basic_shader = Shader("data/shaders/vert_shader.vert", "data/shaders/frag_shader.frag")

    proj_matrix = glm.perspective(glm.radians(45), display.WIDTH / display.HEIGHT, 0.1, 1500)

    _basic_shader.use()
    _basic_shader.upload_uniform_float16("projection_matrix", proj_matrix)
    _basic_shader.stop()

    glEnable(GL_DEPTH_TEST)


def begin(camera: Camera):
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT)

    _basic_shader.use()
    _basic_shader.upload_uniform_float16("view_matrix", camera.view_matrix)


def draw(entity: Entity, position: glm.vec3, rotation: glm.vec3, scale: float):
    _basic_shader.upload_uniform_float16("model_matrix", _create_transformation_matrix(position, rotation, scale))
    _basic_shader.upload_uniform_int1("texture", 0)

    vao = entity.model.vertex_array

    entity.texture.bind(0)

    vao.bind()
    vao.enable_index_buffer()
    glDrawElements(GL_TRIANGLES, vao.vertex_count, GL_UNSIGNED_INT, None)


def set_clear_color(red: float, green: float, blue: float):
    glClearColor(red, green, blue, 1)


def dispose():
    _basic_shader.dispose()


def _create_transformation_matrix(position: glm.vec3, rotation: glm.vec3, scale: float):
    matrix = glm.mat4(1)
    matrix = glm.translate(matrix, position)
    matrix = glm.rotate(matrix, rotation.x, glm.vec3(1, 0, 0))
    matrix = glm.rotate(matrix, rotation.y, glm.vec3(0, 1, 0))
    matrix = glm.rotate(matrix, rotation.z, glm.vec3(0, 0, 1))
    matrix = glm.scale(matrix, glm.vec3(scale, scale, scale))

    return matrix
