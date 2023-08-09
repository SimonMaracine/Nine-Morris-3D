#include <cstddef>
#include <string_view>
#include <memory>
#include <vector>
#include <cstring>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>

#include "engine/application_base/panic.hpp"
#include "engine/other/mesh.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/encrypt.hpp"

namespace sm {
    struct PTN {
        glm::vec3 position {};
        glm::vec2 texture_coordinate {};
        glm::vec3 normal {};
    };

    struct P {
        glm::vec3 position {};
    };

    struct PTNT {
        glm::vec3 position {};
        glm::vec2 texture_coordinate {};
        glm::vec3 normal {};
        glm::vec3 tangent {};
    };

    static void load_PTN(const aiMesh* mesh, std::vector<PTN>& vertices, std::vector<unsigned int>& indices) {
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
    }

    static void load_P(const aiMesh* mesh, std::vector<P>& vertices, std::vector<unsigned int>& indices) {
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
    }

    static void load_PTNT(const aiMesh* mesh, std::vector<PTNT>& vertices, std::vector<unsigned int>& indices) {
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
    }

    Mesh::Mesh(const void* vertices, std::size_t vertices_size, const void* indices, std::size_t indices_size) {
        this->vertices = new char[vertices_size];
        std::memcpy(this->vertices, vertices, vertices_size);
        this->vertices_size = vertices_size;

        this->indices = new unsigned int[indices_size];
        std::memcpy(this->indices, indices, indices_size);
        this->indices_size = indices_size;
    }

    Mesh::~Mesh() {
        delete[] vertices;
        delete[] indices;

        LOG_DEBUG("Freed model data");
    }

    std::shared_ptr<Mesh> Meshes::load_model_PTN(std::string_view file_path, bool flip_winding) {
        LOG_DEBUG("Loading PTN model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTN> vertices;
        std::vector<unsigned int> indices;

        load_PTN(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            vertices.data(),
            vertices.size() * sizeof(PTN),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }

    std::shared_ptr<Mesh> Meshes::load_model_PTN(Encrypt::EncryptedFile file_path, bool flip_winding) {
        LOG_DEBUG("Loading PTN model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        const auto [buffer, buffer_size] = Encrypt::load_file(file_path);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer,
            buffer_size,
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTN> vertices;
        std::vector<unsigned int> indices;

        load_PTN(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            vertices.data(),
            vertices.size() * sizeof(PTN),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }

    std::shared_ptr<Mesh> Meshes::load_model_P(std::string_view file_path, bool flip_winding) {
        LOG_DEBUG("Loading P model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<P> vertices;
        std::vector<unsigned int> indices;

        load_P(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            vertices.data(),
            vertices.size() * sizeof(P),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }

    std::shared_ptr<Mesh> Meshes::load_model_P(Encrypt::EncryptedFile file_path, bool flip_winding) {
        LOG_DEBUG("Loading P model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        const auto [buffer, buffer_size] = Encrypt::load_file(file_path);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer,
            buffer_size,
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<P> vertices;
        std::vector<unsigned int> indices;

        load_P(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            vertices.data(),
            vertices.size() * sizeof(P),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }

    std::shared_ptr<Mesh> Meshes::load_model_PTNT(std::string_view file_path, bool flip_winding) {
        LOG_DEBUG("Loading PTNT model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip | aiProcess_CalcTangentSpace
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTNT> vertices;
        std::vector<unsigned int> indices;

        load_PTNT(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            vertices.data(),
            vertices.size() * sizeof(PTNT),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }

    std::shared_ptr<Mesh> Meshes::load_model_PTNT(Encrypt::EncryptedFile file_path, bool flip_winding) {
        LOG_DEBUG("Loading PTNT model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        const auto [buffer, buffer_size] = Encrypt::load_file(file_path);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer,
            buffer_size,
            aiProcess_ValidateDataStructure | flip | aiProcess_CalcTangentSpace
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTNT> vertices;
        std::vector<unsigned int> indices;

        load_PTNT(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            vertices.data(),
            vertices.size() * sizeof(PTNT),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }
}
