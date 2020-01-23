from src.vertex_array import VertexArray
from load_model import load_model as _load_model

from src.disposable import Disposable


class Model(Disposable):

    def __init__(self, file_path: str):
        super().__init__()

        self.vertex_array = VertexArray()

        positions, texture_coordinates, normals, indices = Model._load(file_path)

        self.vertex_array.add_attribute(0, 3, positions)
        self.vertex_array.add_attribute(1, 2, texture_coordinates)
        # self.vertex_array.add_attribute(2, 3, normals)
        self.vertex_array.add_index_buffer(indices)

    def dispose(self):
        super().dispose()
        self.vertex_array.dispose()

    @staticmethod
    def _load(file_path: str) -> tuple:
        return _load_model(file_path)
