#include <string>
#include <vector>
#include <utility>
#include <string.h>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include "other/model.h"
#include "other/logging.h"

namespace model {
    std::tuple<Mesh, Mesh> load_models(const std::string& file_path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(file_path,
                                                 aiProcess_ValidateDataStructure);

        if (!scene) {
            spdlog::critical("Could not load model {}", file_path.c_str());
            spdlog::critical(importer.GetErrorString());
            std::exit(1);
        }

        const aiNode* root_node = scene->mRootNode;
        
        const aiMesh* board_mesh;
        const aiMesh* piece_mesh;

        for (int i = 0; i < 2; i++) {
            const aiNode* node = root_node->mChildren[i];

            if (strcmp(node->mName.C_Str(), "Board_Cube.001") == 0) {
                board_mesh = scene->mMeshes[node->mMeshes[0]];
            } else if (strcmp(node->mName.C_Str(), "White_Piece_Cylinder") == 0) {
                piece_mesh = scene->mMeshes[node->mMeshes[0]];
            } else {
                spdlog::critical("Could not find meshes");
                std::exit(1);
            }
        }

        const aiMesh* meshes[2] = { board_mesh, piece_mesh };

        Mesh board, piece;

        for (int j = 0; j < 2; j++) {
            std::vector<Vertex> vertices;
            std::vector<unsigned int> indices;

            for (unsigned int i = 0; i < meshes[j]->mNumVertices; i++) {
                Vertex vertex;

                glm::vec3 position;
                position.x = meshes[j]->mVertices[i].x;
                position.y = meshes[j]->mVertices[i].y;
                position.z = meshes[j]->mVertices[i].z;
                vertex.position = position;

                glm::vec2 texture_coordinate;
                texture_coordinate.x = meshes[j]->mTextureCoords[0][i].x;
                texture_coordinate.y = meshes[j]->mTextureCoords[0][i].y;
                vertex.texture_coordinate = texture_coordinate;

                glm::vec3 normal;
                normal.x = meshes[j]->mNormals[i].x;
                normal.y = meshes[j]->mNormals[i].y;
                normal.z = meshes[j]->mNormals[i].z;
                vertex.normal = normal;

                vertices.push_back(vertex);
            }
            
            for (unsigned int i = 0; i < meshes[j]->mNumFaces; i++) {
                aiFace face = meshes[j]->mFaces[i];
                for (unsigned int j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }

            switch (j) {
                case 0:
                    board = { Board, vertices, indices };
                    break;
                case 1:
                    piece = { Piece, vertices, indices };
                    break;
            }
        }

        return std::make_tuple(board, piece);
    }
}
