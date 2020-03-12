import glm


class Node:

    def __init__(self, position: glm.vec3):
        self.position = position
        self.piece = None
