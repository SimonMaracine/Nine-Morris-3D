import glm

from engine import renderer, Entity, Model, Texture

BOARD_ORIGIN = glm.vec3(-2.9, 1.46, -3.8)
WHITE = "WHITE"
BLACK = "BLACK"

_model = Model("data/models/piece.obj")
_white_texture = Texture("data/textures/white_piece_texture.png")
_black_texture = Texture("data/textures/black_piece_texture.png")


class Piece(Entity):

    def __init__(self, x: float, y: float, color: int):
        super().__init__(_model, _white_texture if color == WHITE else _black_texture)

        self.position = BOARD_ORIGIN + glm.vec3(x, 0, y)  # position in 3D space
        self.scale = 0.2

        self.board_pos = (x, y)

    def render(self):
        renderer.draw(self, self.position, self.rotation, self.scale)

    def dispose(self):
        super().dispose()
