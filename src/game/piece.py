import glm

from engine import renderer, Entity, Model, Texture

WHITE = 0
BLACK = 1

_model = Model("data/models/piece.obj")
_white_texture = Texture("data/textures/white_piece_texture.png")
_black_texture = Texture("data/textures/black_piece_texture.png")


class Piece(Entity):

    def __init__(self, position: glm.vec3, color: int):
        super().__init__(_model, _white_texture if color == WHITE else _black_texture)
        self.position = position

    def render(self):
        renderer.draw(self, self.position, self.rotation, 0.2)

    def dispose(self):
        super().dispose()
