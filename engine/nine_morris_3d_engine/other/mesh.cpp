#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <cppblowfish.h>

#include "nine_morris_3d_engine/other/mesh.h"
#include "nine_morris_3d_engine/other/logging.h"
#include "nine_morris_3d_engine/other/encrypt.h"

namespace mesh {
    std::shared_ptr<Mesh<VPTN>> load_model_VPTN(std::string_view file_path, bool flip_winding_order) {
        DEB_DEBUG("Loading VPTN model '{}'...", file_path);

        const aiPostProcessSteps flip = flip_winding_order ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            REL_CRITICAL("Could not load model '{}', exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            exit(1);
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<VPTN> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            VPTN vertex;

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

        return std::make_shared<Mesh<VPTN>>(vertices, indices);
    }

    std::shared_ptr<Mesh<VPTN>> load_model_VPTN(encrypt::EncryptedFile file_path, bool flip_winding_order) {
        DEB_DEBUG("Loading VPTN model '{}'...", file_path);

        cppblowfish::Buffer buffer = encrypt::load_file(file_path);

        const aiPostProcessSteps flip = flip_winding_order ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer.get(),
            buffer.size() - buffer.padding(),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            REL_CRITICAL("Could not load model '{}', exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            exit(1);
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<VPTN> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            VPTN vertex;

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

        return std::make_shared<Mesh<VPTN>>(vertices, indices);
    }

    std::shared_ptr<Mesh<VP>> load_model_VP(std::string_view file_path, bool flip_winding_order) {
        DEB_DEBUG("Loading VP model '{}'...", file_path);

        const aiPostProcessSteps flip = flip_winding_order ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            REL_CRITICAL("Could not load model '{}', exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            exit(1);
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<VP> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            VP vertex;

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

        return std::make_shared<Mesh<VP>>(vertices, indices);
    }

    std::shared_ptr<Mesh<VP>> load_model_VP(encrypt::EncryptedFile file_path, bool flip_winding_order) {
        DEB_DEBUG("Loading VP model '{}'...", file_path);

        cppblowfish::Buffer buffer = encrypt::load_file(file_path);

        const aiPostProcessSteps flip = flip_winding_order ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer.get(),
            buffer.size() - buffer.padding(),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            REL_CRITICAL("Could not load model '{}', exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            exit(1);
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<VP> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            VP vertex;

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

        return std::make_shared<Mesh<VP>>(vertices, indices);
    }

    std::shared_ptr<Mesh<VPTNT>> load_model_VPTNT(std::string_view file_path, bool flip_winding_order) {
        DEB_DEBUG("Loading VPTNT model '{}'...", file_path);

        const aiPostProcessSteps flip = flip_winding_order ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip | aiProcess_CalcTangentSpace
        );

        if (!scene) {
            REL_CRITICAL("Could not load model '{}', exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            exit(1);
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<VPTNT> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            VPTNT vertex;

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

        return std::make_shared<Mesh<VPTNT>>(vertices, indices);
    }

    std::shared_ptr<Mesh<VPTNT>> load_model_VPTNT(encrypt::EncryptedFile file_path, bool flip_winding_order) {
        DEB_DEBUG("Loading VPTNT model '{}'...", file_path);

        cppblowfish::Buffer buffer = encrypt::load_file(file_path);

        const aiPostProcessSteps flip = flip_winding_order ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer.get(),
            buffer.size() - buffer.padding(),
            aiProcess_ValidateDataStructure | flip | aiProcess_CalcTangentSpace
        );

        if (!scene) {
            REL_CRITICAL("Could not load model '{}', exiting...", file_path);
            REL_CRITICAL(importer.GetErrorString());
            exit(1);
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<VPTNT> vertices;
        std::vector<unsigned int> indices;

        for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
            VPTNT vertex;

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

        return std::make_shared<Mesh<VPTNT>>(vertices, indices);
    }
}
