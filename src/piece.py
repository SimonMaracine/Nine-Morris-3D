import glm

import src.renderer as renderer
from src.entity import Entity
import src.assets as assets

WHITE = 0
BLACK = 1


class Piece(Entity):

    def __init__(self, position: glm.vec3, color: int):
        super().__init__(assets.piece_model, assets.white_piece_texture
                         if color == WHITE else
                         assets.black_piece_texture)
        self.position = position

    def render(self):
        renderer.draw(self, self.position, self.rotation, 0.2)

    def dispose(self):
        super().dispose()
