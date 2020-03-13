import glm

from engine.model import Model
from engine.texture import Texture


class Entity:
    def __init__(self, model: Model, texture: Texture):
        self.model = model
        self.texture = texture

        self.position = glm.vec3()
        self.rotation = glm.vec3()
        self.scale = 1

    def dispose(self):
        self.model.dispose()
        self.texture.dispose()
