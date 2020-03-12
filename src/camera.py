from math import sin, cos, radians

import glm
from pyglfw.libapi import *

import src.display as display
from src.input import get_key_pressed


class Camera:

    def __init__(self, position: glm.vec3):
        self.position = position

        self.pitch = 0
        self.yaw = 0

        self.view_matrix = glm.mat4(1)
        self._look_direction = glm.vec3()

        self._last_mouse_x = display.WIDTH // 2
        self._last_mouse_y = display.HEIGHT // 2

    def update(self):
        if self.pitch > 89:
            self.pitch = 89
        if self.pitch < -89:
            self.pitch = -89

        self._look_direction.x = cos(radians(self.pitch)) * cos(radians(self.yaw))
        self._look_direction.y = sin(radians(self.pitch))
        self._look_direction.z = cos(radians(self.pitch)) * sin(radians(self.yaw))

        if get_key_pressed(GLFW_KEY_W):
            self.position += glm.normalize(self._look_direction) * 0.4
        elif get_key_pressed(GLFW_KEY_S):
            self.position -= glm.normalize(self._look_direction) * 0.4
        elif get_key_pressed(GLFW_KEY_A):
            self.position -= glm.normalize(glm.cross(self._look_direction, glm.vec3(0, 1, 0))) * 0.4
        elif get_key_pressed(GLFW_KEY_D):
            self.position += glm.normalize(glm.cross(self._look_direction, glm.vec3(0, 1, 0))) * 0.4

        self.view_matrix = glm.lookAt(self.position, self.position + self._look_direction, glm.vec3(0, 1, 0))

    def update_look_direction(self, mouse_x_pos: float, mouse_y_pos: float, change_look: bool):
        x_offset = (mouse_x_pos - self._last_mouse_x) * 0.1
        y_offset = (self._last_mouse_y - mouse_y_pos) * 0.1

        self._last_mouse_x = mouse_x_pos
        self._last_mouse_y = mouse_y_pos

        if change_look:
            self.pitch -= y_offset
            self.yaw -= x_offset
