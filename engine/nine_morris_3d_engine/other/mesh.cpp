#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <cppblowfish/cppblowfish.h>

#include "nine_morris_3d_engine/other/mesh.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/encrypt.h"
#include "nine_morris_3d_engine/other/exit.h"

namespace mesh {
    std::shared_ptr<Mesh<PTN>> load_model_PTN(std::string_view file_path, bool flip_winding) {
        DEB_DEBUG("Loading PTN model `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            REL_CRITICAL("Could not load model `{}`, exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            game_exit::exit_critical();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTN> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            PTN vertex;

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

        return std::make_shared<Mesh<PTN>>(vertices, indices);
    }

    std::shared_ptr<Mesh<PTN>> load_model_PTN(encrypt::EncryptedFile file_path, bool flip_winding) {
        DEB_DEBUG("Loading PTN model `{}`...", file_path);

        const cppblowfish::Buffer buffer = encrypt::load_file(file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer.get(),
            buffer.size() - buffer.padding(),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            REL_CRITICAL("Could not load model `{}`, exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            game_exit::exit_critical();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTN> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            PTN vertex;

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

        return std::make_shared<Mesh<PTN>>(vertices, indices);
    }

    std::shared_ptr<Mesh<P>> load_model_P(std::string_view file_path, bool flip_winding) {
        DEB_DEBUG("Loading P model `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            REL_CRITICAL("Could not load model `{}`, exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            game_exit::exit_critical();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<P> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            P vertex;

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

        return std::make_shared<Mesh<P>>(vertices, indices);
    }

    std::shared_ptr<Mesh<P>> load_model_P(encrypt::EncryptedFile file_path, bool flip_winding) {
        DEB_DEBUG("Loading P model `{}`...", file_path);

        const cppblowfish::Buffer buffer = encrypt::load_file(file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer.get(),
            buffer.size() - buffer.padding(),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            REL_CRITICAL("Could not load model `{}`, exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            game_exit::exit_critical();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<P> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            P vertex;

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

        return std::make_shared<Mesh<P>>(vertices, indices);
    }

    std::shared_ptr<Mesh<PTNT>> load_model_PTNT(std::string_view file_path, bool flip_winding) {
        DEB_DEBUG("Loading PTNT model `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip | aiProcess_CalcTangentSpace
        );

        if (!scene) {
            REL_CRITICAL("Could not load model `{}`, exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            game_exit::exit_critical();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTNT> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            PTNT vertex;

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

            glm::vec3 tangent;
            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;
            vertex.tangent = tangent;

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return std::make_shared<Mesh<PTNT>>(vertices, indices);
    }

    std::shared_ptr<Mesh<PTNT>> load_model_PTNT(encrypt::EncryptedFile file_path, bool flip_winding) {
        DEB_DEBUG("Loading PTNT model `{}`...", file_path);

        const cppblowfish::Buffer buffer = encrypt::load_file(file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer.get(),
            buffer.size() - buffer.padding(),
            aiProcess_ValidateDataStructure | flip | aiProcess_CalcTangentSpace
        );

        if (!scene) {
            REL_CRITICAL("Could not load model `{}`, exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            game_exit::exit_critical();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTNT> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            PTNT vertex;

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

            glm::vec3 tangent;
            tangent.x = mesh->mTangents[i].x;
            tangent.y = mesh->mTangents[i].y;
            tangent.z = mesh->mTangents[i].z;
            vertex.tangent = tangent;

            vertices.push_back(vertex);
        }

        for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
            aiFace face = mesh->mFaces[i];
            for (unsigned int j = 0; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }

        return std::make_shared<Mesh<PTNT>>(vertices, indices);
    }
}
