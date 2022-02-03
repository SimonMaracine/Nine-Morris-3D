#include <string>
#include <vector>
#include <memory>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include "other/model.h"
#include "other/logging.h"

namespace model {
    std::shared_ptr<Mesh<Vertex>> load_model(const std::string& file_path) {
        DEB_DEBUG("Loading model '{}'...", file_path.c_str());

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(file_path, aiProcess_ValidateDataStructure);

        if (!scene) {
            REL_CRITICAL("Could not load model '{}'", file_path.c_str());
            REL_CRITICAL(importer.GetErrorString());
            std::exit(1);
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

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

            glm::vec3 normal;
            normal.x = mesh->mNormals[i].x;
            normal.y = mesh->mNormals[i].y;
            normal.z = mesh->mNormals[i].z;
            vertex.normal = normal;

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return std::make_shared<Mesh<Vertex>>(vertices, indices);
    }

    std::shared_ptr<Mesh<VertexP>> load_model_position(const std::string& file_path) {
        DEB_DEBUG("Loading model '{}'...", file_path.c_str());

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(file_path, aiProcess_ValidateDataStructure);

        if (!scene) {
            REL_CRITICAL("Could not load model '{}'", file_path.c_str());
            REL_CRITICAL(importer.GetErrorString());
            std::exit(1);
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<VertexP> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            VertexP vertex;

            glm::vec3 position;
            position.x = mesh->mVertices[i].x;
            position.y = mesh->mVertices[i].y;
            position.z = mesh->mVertices[i].z;
            vertex.position = position;

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return std::make_shared<Mesh<VertexP>>(vertices, indices);
    }
}
