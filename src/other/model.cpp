#include <string>
#include <vector>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include "other/model.h"
#include "logging.h"

namespace model {
    Mesh load_model(const std::string& file_path) {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(file_path,
                                                 aiProcess_ValidateDataStructure);

        if (!scene) {
            SPDLOG_CRITICAL("Could not load model {}", file_path.c_str());
            SPDLOG_CRITICAL(importer.GetErrorString());
            std::exit(1);
        }

        const aiNode* root_node = scene->mRootNode;
        const aiNode* node = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[node->mMeshes[0]];

        std::vector<Vertex> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            Vertex vertex;

            glm::vec3 position;
            position.x = mesh->mVertices[i].x;
            position.y = mesh->mVertices[i].y;
            position.z = mesh->mVertices[i].z;
            vertex.position = position;

            glm::vec2 texture_coordinate;
            texture_coordinate.x = mesh->mTextureCoords[0][i].x;
            texture_coordinate.y = mesh->mTextureCoords[0][i].y;
            vertex.texture_coordinate = texture_coordinate;

            vertices.push_back(vertex);
        }
        
        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return { vertices, indices };
    }
}
