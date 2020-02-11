import glm

from src.model import Model
from src.texture import Texture
from src.disposable import Disposable


class Entity(Disposable):
    def __init__(self, model: Model, texture: Texture):
        super().__init__()

        self.model = model
        self.texture = texture

        self.position = glm.vec3()
        self.rotation = glm.vec3()
        self.scale = 1

    def dispose(self):
        super().dispose()
        self.model.dispose()
        self.texture.dispose()
