from engine import renderer, Entity, Model, Texture
from game.node import Node
from game.piece import Piece, WHITE, BLACK

GRID = 1.065

_model = Model("data/models/board.obj")
_texture = Texture("data/textures/board_texture.png")


class Board(Entity):

    def __init__(self):
        super().__init__(_model, _texture)

        self.nodes = (
            Node(0, 0), Node(0, GRID * 3), Node(0, GRID * 6)
        )
        self.pieces = [
            Piece(0, 0, BLACK),
            Piece(0, GRID * 3, BLACK),
            Piece(0, GRID * 6, BLACK),
            Piece(GRID * 6, 0, WHITE)
        ]

    def update(self, mouse_ray):
        for node in self.nodes:
            node.update(mouse_ray)

    def render(self):
        renderer.draw(self, self.position, self.rotation, self.scale)
        for piece in self.pieces:
            piece.render()
        for node in self.nodes:
            node.render()

    def dispose(self):
        super().dispose()
        for piece in self.pieces:
            piece.dispose()
        for node in self.nodes:
            node.dispose()
