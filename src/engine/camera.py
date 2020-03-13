from math import sin, cos, radians

import glm
from pyglfw.libapi import *

from engine import display, input


class Camera:

    def __init__(self, position: glm.vec3):
        self.position = position

        self.pitch = 0
        self.yaw = 0

        self.distance_to_point = 20
        self.angle_around_point = 0

        self.view_matrix = glm.mat4(1)
        self._look_direction = glm.vec3()

    def update(self):
        if self.pitch > 89:
            self.pitch = 89
        if self.pitch < -89:
            self.pitch = -89

        if input.get_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT):
            self.angle_around_point -= input.get_mouse_rel_x() / 2

        horizontal_distance = self.distance_to_point * cos(radians(self.pitch))
        vertical_distance = self.distance_to_point * sin(radians(self.pitch))



        self._look_direction.x = cos(radians(self.pitch)) * cos(radians(self.yaw))
        self._look_direction.y = sin(radians(self.pitch))
        self._look_direction.z = cos(radians(self.pitch)) * sin(radians(self.yaw))

        if input.get_key_pressed(GLFW_KEY_W):
            self.position += glm.normalize(self._look_direction) * 0.4
        elif input.get_key_pressed(GLFW_KEY_S):
            self.position -= glm.normalize(self._look_direction) * 0.4
        elif input.get_key_pressed(GLFW_KEY_A):
            self.position -= glm.normalize(glm.cross(self._look_direction, glm.vec3(0, 1, 0))) * 0.4
        elif input.get_key_pressed(GLFW_KEY_D):
            self.position += glm.normalize(glm.cross(self._look_direction, glm.vec3(0, 1, 0))) * 0.4

        self.view_matrix = glm.lookAt(self.position, self.position + self._look_direction, glm.vec3(0, 1, 0))

    def update_look_direction(self):
        x_offset = input.get_mouse_rel_x() * 0.1  # these should always be called to update the internal variables
        y_offset = -input.get_mouse_rel_y() * 0.1

        if input.get_mouse_button_pressed(GLFW_MOUSE_BUTTON_LEFT):
            self.pitch -= y_offset
            self.yaw -= x_offset
