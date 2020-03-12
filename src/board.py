import glm

import src.renderer as renderer
from src.node import Node
from src.piece import Piece, WHITE, BLACK
from src.entity import Entity
import src.assets as assets

ORIGIN = glm.vec3(-2.9, 1.5, -3.8)
GRID = 1.065


class Board(Entity):

    def __init__(self):
        super().__init__(assets.board_model, assets.board_texture)

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
