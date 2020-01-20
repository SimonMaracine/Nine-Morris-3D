from math import sin, cos, radians

import glm
from pyglfw.libapi import *

import src.display as display


class Camera:

    def __init__(self, position: glm.vec3):
        self.position = position

        self.pitch = 0
        self.yaw = 0

        self.look_direction = glm.vec3()
        self.view_matrix = glm.mat4(1)

    def update(self):
        self.look_direction.x = cos(radians(self.pitch) * cos(radians(self.yaw)))
        self.look_direction.y - sin(radians(self.pitch))
        self.look_direction.z = cos(radians(self.pitch) * sin(radians(self.yaw)))

        if glfwGetKey(display.windowp, GLFW_KEY_W) == GLFW_PRESS:
            self.position += glm.normalize(self.look_direction) * 2
        elif glfwGetKey(display.windowp, GLFW_KEY_S) == GLFW_PRESS:
            self.position -= glm.normalize(self.look_direction) * 2
        elif glfwGetKey(display.windowp, GLFW_KEY_A) == GLFW_PRESS:
            self.position -= glm.normalize(glm.cross(self.look_direction, glm.vec3(0, 1, 0))) * 2
        elif glfwGetKey(display.windowp, GLFW_KEY_D) == GLFW_PRESS:
            self.position += glm.normalize(glm.cross(self.look_direction, glm.vec3(0, 1, 0))) * 2

        self.view_matrix = glm.lookAt(self.position, self.position + self.look_direction, glm.vec3(0, 1, 0))
