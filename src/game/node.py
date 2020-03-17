from typing import Optional

import glm

from engine import renderer, Entity, Model, Texture
from game.piece import Piece, BOARD_ORIGIN

_model = Model("data/models/node_highlight.obj")
_texture = Texture("data/textures/whiteish.png")


class Node(Entity):

    def __init__(self, x: float, y: float):
        super().__init__(_model, _texture)

        self.position = BOARD_ORIGIN + glm.vec3(x, 0.2, y)  # position in 3D space
        self.scale = 0.3

        self.board_pos = (x, y)
        self.radius = 2

        self.piece: Optional[Piece] = None
        self.highlight = False

    def render(self):
        if self.highlight:
            renderer.draw(self, self.position, self.rotation, self.scale)

    def update(self, ray: glm.vec3):  # TODO doesn't work
        t = glm.dot(self.position, ray)
        p = ray * t
        y = glm.length(self.position - p)

        if y < self.radius:
            self.highlight = True
            print("INTERSECTING")
        else:
            self.highlight = False
