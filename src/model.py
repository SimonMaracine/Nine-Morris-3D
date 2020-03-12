import pyassimp

from src.vertex_array import VertexArray
# from load_model import load_model as _load_model


class Model:

    def __init__(self, file_path: str):
        self.vertex_array = VertexArray()

        positions, texture_coordinates, normals, indices = Model._load(file_path)

        self.vertex_array.add_attribute(0, 3, positions)
        self.vertex_array.add_attribute(1, 2, texture_coordinates)
        # self.vertex_array.add_attribute(2, 3, normals)
        self.vertex_array.add_index_buffer(indices)

    def dispose(self):
        self.vertex_array.dispose()

    @staticmethod
    def _load(file_path: str) -> tuple:
        scene = pyassimp.load(file_path)
        mesh = scene.meshes[0]

        vertices = []
        for i in range(len(mesh.vertices)):
            vertices.append(mesh.vertices[i][0])
            vertices.append(mesh.vertices[i][1])
            vertices.append(mesh.vertices[i][2])

        texture_coordinates = []
        for i in range(len(mesh.texturecoords[0])):
            texture_coordinates.append(mesh.texturecoords[0][i][0])
            texture_coordinates.append(mesh.texturecoords[0][i][1])

        normals = []
        for i in range(len(mesh.normals)):
            normals.append(mesh.normals[i][0])
            normals.append(mesh.normals[i][1])
            normals.append(mesh.normals[i][2])

        indices = []
        for i in range(len(mesh.faces)):
            face = mesh.faces[i]
            indices.append(face[0])
            indices.append(face[1])
            indices.append(face[2])

        pyassimp.release(scene)
        return vertices, texture_coordinates, normals, indices
        # return _load_model(file_path)
