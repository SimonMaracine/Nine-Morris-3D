#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <glm/glm.hpp>
#include <cppblowfish/cppblowfish.h>

#include "engine/other/mesh.h"
#include "engine/other/logging.h"
#include "engine/other/encrypt.h"
#include "engine/application_base/panic.h"

struct PTN {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec2 texture_coordinate = glm::vec2(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
};

struct P {
    glm::vec3 position = glm::vec3(0.0f);
};

struct PTNT {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec2 texture_coordinate = glm::vec2(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);
    glm::vec3 tangent = glm::vec3(0.0f);
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

namespace mesh {
    Mesh::Mesh(const char* vertices, size_t vertices_size, const unsigned int* indices, size_t indices_size) {
        this->vertices = new char[vertices_size];
        memcpy(this->vertices, vertices, vertices_size);
        this->vertices_size = vertices_size;

        this->indices = new unsigned int[indices_size];
        memcpy(this->indices, indices, indices_size);
        this->indices_size = indices_size;
    }

    Mesh::~Mesh() {
        delete[] vertices;
        delete[] indices;

        LOG_DEBUG("Freed model data");
    }

    std::shared_ptr<Mesh> load_model_PTN(std::string_view file_path, bool flip_winding) {
        LOG_DEBUG("Loading PTN model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`, exiting...", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic::panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTN> vertices;
        std::vector<unsigned int> indices;

        load_PTN(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            reinterpret_cast<const char*>(vertices.data()),
            vertices.size() * sizeof(PTN),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }

    std::shared_ptr<Mesh> load_model_PTN(Encrypt::EncryptedFile file_path, bool flip_winding) {
        LOG_DEBUG("Loading PTN model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        const cppblowfish::Buffer buffer = Encrypt::load_file(file_path);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer.get(),
            buffer.size() - buffer.padding(),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`, exiting...", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic::panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTN> vertices;
        std::vector<unsigned int> indices;

        load_PTN(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            reinterpret_cast<const char*>(vertices.data()),
            vertices.size() * sizeof(PTN),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }

    std::shared_ptr<Mesh> load_model_P(std::string_view file_path, bool flip_winding) {
        LOG_DEBUG("Loading P model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`, exiting...", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic::panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<P> vertices;
        std::vector<unsigned int> indices;

        load_P(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            reinterpret_cast<const char*>(vertices.data()),
            vertices.size() * sizeof(P),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }

    std::shared_ptr<Mesh> load_model_P(Encrypt::EncryptedFile file_path, bool flip_winding) {
        LOG_DEBUG("Loading P model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        const cppblowfish::Buffer buffer = Encrypt::load_file(file_path);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer.get(),
            buffer.size() - buffer.padding(),
            aiProcess_ValidateDataStructure | flip
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`, exiting...", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic::panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<P> vertices;
        std::vector<unsigned int> indices;

        load_P(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            reinterpret_cast<const char*>(vertices.data()),
            vertices.size() * sizeof(P),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }

    std::shared_ptr<Mesh> load_model_PTNT(std::string_view file_path, bool flip_winding) {
        LOG_DEBUG("Loading PTNT model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(
            std::string(file_path),
            aiProcess_ValidateDataStructure | flip | aiProcess_CalcTangentSpace
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`, exiting...", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic::panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTNT> vertices;
        std::vector<unsigned int> indices;

        load_PTNT(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            reinterpret_cast<const char*>(vertices.data()),
            vertices.size() * sizeof(PTNT),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }

    std::shared_ptr<Mesh> load_model_PTNT(Encrypt::EncryptedFile file_path, bool flip_winding) {
        LOG_DEBUG("Loading PTNT model data `{}`...", file_path);

        const aiPostProcessSteps flip = flip_winding ? aiProcess_FlipWindingOrder : static_cast<aiPostProcessSteps>(0);

        const cppblowfish::Buffer buffer = Encrypt::load_file(file_path);

        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFileFromMemory(
            buffer.get(),
            buffer.size() - buffer.padding(),
            aiProcess_ValidateDataStructure | flip | aiProcess_CalcTangentSpace
        );

        if (!scene) {
            LOG_DIST_CRITICAL("Could not load model data `{}`, exiting...", file_path);
            LOG_DIST_CRITICAL(importer.GetErrorString());
            panic::panic();
        }

        const aiNode* root_node = scene->mRootNode;

        const aiNode* collection = root_node->mChildren[0];
        const aiMesh* mesh = scene->mMeshes[collection->mMeshes[0]];

        std::vector<PTNT> vertices;
        std::vector<unsigned int> indices;

        load_PTNT(mesh, vertices, indices);

        return std::make_shared<Mesh>(
            reinterpret_cast<const char*>(vertices.data()),
            vertices.size() * sizeof(PTNT),
            indices.data(),
            indices.size() * sizeof(unsigned int)
        );
    }
}
