import math
import glm

import src.input as input
import src.display as display
from src.camera import Camera


class MouseRay:

    def __init__(self, camera: Camera, projection_matrix: glm.mat4):
        self.current_ray = glm.vec3()
        self.view_matrix = glm.mat4(1)

        self.camera = camera
        self.projection_matrix = projection_matrix

    def update(self):
        self.view_matrix = self._create_view_matrix()

        mouse_x = input.get_mouse_poition()[0]
        mouse_y = input.get_mouse_poition()[1]

        normalized_coordinates = self._convert_to_normalized_device_coordinates(mouse_x, mouse_y)
        clip_coordinates = glm.vec4(normalized_coordinates.x, normalized_coordinates.y, -1, 1)
        eye_coordinates = self._convert_to_eye_coordinates(clip_coordinates)
        self.current_ray = self._convert_to_world_coordinates(eye_coordinates)

    def _convert_to_normalized_device_coordinates(self, mouse_x: float, mouse_y: float) -> glm.vec2:
        x = (2 * mouse_x) / display.WIDTH - 1
        y = (2 * mouse_y) / display.HEIGHT - 1
        return glm.vec2(x, y)

    def _convert_to_eye_coordinates(self, vector: glm.vec4) -> glm.vec4:
        inverted_projection = glm.inverse(self.projection_matrix)
        eye_coordinates = inverted_projection * vector
        return glm.vec4(eye_coordinates.x, eye_coordinates.y, -1, 0)

    def _convert_to_world_coordinates(self, vector: glm.vec4) -> glm.vec3:
        inverted_view = glm.inverse(self.view_matrix)
        world_coordinates = inverted_view * vector
        return glm.normalize(glm.vec3(world_coordinates.x, world_coordinates.y, world_coordinates.z))

    def _create_view_matrix(self) -> glm.mat4:
        view_matrix = glm.mat4(1)

        view_matrix = glm.rotate(view_matrix, math.radians(self.camera.pitch), glm.vec3(1, 0, 0))
        view_matrix = glm.rotate(view_matrix, math.radians(self.camera.yaw), glm.vec3(0, 1,0))
        camera_position = self.camera.position
        negative_camera_position = glm.vec3(-camera_position.x, -camera_position.y, -camera_position.z)
        view_matrix = glm.translate(view_matrix, negative_camera_position)

        return view_matrix
