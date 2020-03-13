import glm

from engine import renderer, Entity, Model, Texture
from game.node import Node
from game.piece import Piece, WHITE, BLACK

ORIGIN = glm.vec3(-2.9, 1.5, -3.8)
GRID = 1.065

_model = Model("data/models/board.obj")
_texture = Texture("data/textures/board_texture.png")


class Board(Entity):

    def __init__(self):
        super().__init__(_model, _texture)

        self.nodes = (
            Node(ORIGIN), Node(ORIGIN + glm.vec3(GRID * 3,  0, 0))
        )
        self.pieces = [
            Piece(ORIGIN + glm.vec3(0, 0, 0), BLACK),
            Piece(ORIGIN + glm.vec3(GRID * 6,  0, 0), WHITE),
        ]

    def render(self):
        renderer.draw(self, self.position, self.rotation, self.scale)
        for piece in self.pieces:
            piece.render()

    def dispose(self):
        super().dispose()
        for piece in self.pieces:
            piece.dispose()
