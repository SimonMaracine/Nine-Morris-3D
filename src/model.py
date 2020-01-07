import pyassimp

from vertex_array import VertexArray


class Model:

    def __init__(self, file_path: str):  # positions: list, texture_coordinates: list, normals: list, indices: list
        self.vertex_array = VertexArray()

        positions, texture_coordinates, normals, indices = Model._load(file_path)

        print(positions)

        self.vertex_array.add_attribute(0, 3, positions)
        self.vertex_array.add_attribute(1, 2, texture_coordinates)
        # self.vertex_array.add_attribute(2, 3, normals)
        self.vertex_array.add_index_buffer(indices)

    def dispose(self):
        self.vertex_array.dispose()

    @staticmethod
    def _load(file_path: str) -> tuple:
        scene = pyassimp.load(file_path)  # TODO this crashes
        assert len(scene.meshes)

        mesh = scene.meshes[0]

        positions = mesh.vertices
        texture_coordinates = mesh.textureCoords
        normals = mesh.normals
        indices = mesh.numIndices

        return positions, texture_coordinates, normals, indices
