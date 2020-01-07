from src.model import Model
from src.texture import Texture


class Entity:

    def __init__(self, model: Model, texture: Texture):
        self.model = model
        self.texture = texture
