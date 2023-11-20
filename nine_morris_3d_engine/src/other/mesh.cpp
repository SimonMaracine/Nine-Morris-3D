#include <cstddef>
#include <string>
#include <vector>
#include <cstring>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "engine/application_base/panic.hpp"
#include "engine/other/mesh.hpp"
#include "engine/other/logging.hpp"
#include "engine/other/encrypt.hpp"

namespace sm {
    struct VertexP {
        aiVector3D position;
    };

    struct VertexPN {
        aiVector3D position;
        aiVector3D normal;
    };

    struct VertexPTN {
        aiVector3D position;
        aiVector2D texture_coordinate;
        aiVector3D normal;
    };

    struct VertexPTNT {
        aiVector3D position;
        aiVector2D texture_coordinate;
        aiVector3D normal;
        aiVector3D tangent;
    };

    static void load_P(const aiMesh* mesh, std::vector<VertexP>& vertices, std::vector<unsigned int>& indices) {
        for (unsigned int i {0}; i < mesh->mNumVertices; i++) {
            VertexP vertex;
            vertex.position = mesh->mVertices[i];

            vertices.push_back(vertex);
        }

        for (unsigned int i {0}; i < mesh->mNumFaces; i++) {
            const aiFace face {mesh->mFaces[i]};

            for (unsigned int j {0}; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    static void load_PN(const aiMesh* mesh, std::vector<VertexPN>& vertices, std::vector<unsigned int>& indices) {
        for (unsigned int i {0}; i < mesh->mNumVertices; i++) {
            VertexPN vertex;
            vertex.position = mesh->mVertices[i];
            vertex.normal = mesh->mNormals[i];

            vertices.push_back(vertex);
        }

        for (unsigned int i {0}; i < mesh->mNumFaces; i++) {
            const aiFace face {mesh->mFaces[i]};

            for (unsigned int j {0}; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    static void load_PTN(const aiMesh* mesh, std::vector<VertexPTN>& vertices, std::vector<unsigned int>& indices) {
        for (unsigned int i {0}; i < mesh->mNumVertices; i++) {
            VertexPTN vertex;
            vertex.position = mesh->mVertices[i];
            vertex.texture_coordinate.x = mesh->mTextureCoords[0][i].x;
            vertex.texture_coordinate.y = mesh->mTextureCoords[0][i].y;
            vertex.normal = mesh->mNormals[i];

            vertices.push_back(vertex);
        }

        for (unsigned int i {0}; i < mesh->mNumFaces; i++) {
            const aiFace face {mesh->mFaces[i]};

            for (unsigned int j {0}; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    static void load_PTNT(const aiMesh* mesh, std::vector<VertexPTNT>& vertices, std::vector<unsigned int>& indices) {
        for (unsigned int i {0}; i < mesh->mNumVertices; i++) {
            VertexPTNT vertex;
            vertex.position = mesh->mVertices[i];
            vertex.texture_coordinate.x = mesh->mTextureCoords[0][i].x;
            vertex.texture_coordinate.y = mesh->mTextureCoords[0][i].y;
            vertex.normal = mesh->mNormals[i];
            vertex.tangent = mesh->mTangents[i];

            vertices.push_back(vertex);
        }

        for (unsigned int i {0}; i < mesh->mNumFaces; i++) {
            const aiFace face {mesh->mFaces[i]};

            for (unsigned int j {0}; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    static const aiMesh* find_mesh(const aiNode* node, const std::string& object_name, const aiScene* scene) {
        for (unsigned int i {0}; i < node->mNumMeshes; i++) {
            const aiMesh* mesh {scene->mMeshes[node->mMeshes[i]]};

            if (std::strcmp(mesh->mName.C_Str(), object_name.c_str()) == 0) {
                return mesh;
            }
        }

        for (unsigned int i {0}; i < node->mNumChildren; i++) {
            return find_mesh(node->mChildren[i], object_name, scene);
        }

        return nullptr;
    }

    Mesh::Mesh(const std::string& file_path, const std::string& object_name, Type type, bool flip_winding) {
        unsigned int flags {aiProcess_ValidateDataStructure};

        if (flip_winding) {
            flags |= aiProcess_FlipWindingOrder;
        }

        if (type == Type::PTNT) {
            flags |= aiProcess_CalcTangentSpace;
        }

        if (type != Type::P) {
            flags |= aiProcess_GenNormals;
        }

        Assimp::Importer importer;

        const aiScene* scene {importer.ReadFile(file_path, flags)};

        if (scene == nullptr) {
            LOG_DIST_CRITICAL("Could not load model data `{}`", file_path);
            LOG_DIST_CRITICAL("Assimp: ", importer.GetErrorString());
            throw ResourceLoadingError;
        }

        const aiNode* root_node {scene->mRootNode};
        const aiMesh* mesh {find_mesh(root_node, object_name, scene)};

        if (mesh == nullptr) {
            LOG_CRITICAL("Model file `{}` does not contain `{}` mesh", file_path, object_name);
            throw ResourceLoadingError;
        }

        load(type, mesh, file_path);
    }

    Mesh::Mesh(const EncrFile& file_path, const std::string& object_name, Type type, bool flip_winding) {
        unsigned int flags {aiProcess_ValidateDataStructure};

        if (flip_winding) {
            flags |= aiProcess_FlipWindingOrder;
        }

        if (type == Type::PTNT) {
            flags |= aiProcess_CalcTangentSpace;
        }

        if (type != Type::P) {
            flags |= aiProcess_GenNormals;
        }

        const auto [buffer, buffer_size] {Encrypt::load_file(file_path)};

        Assimp::Importer importer;

        const aiScene* scene {importer.ReadFileFromMemory(buffer, buffer_size, flags)};

        if (scene == nullptr) {
            LOG_DIST_CRITICAL("Could not load model data `{}`", file_path);
            LOG_DIST_CRITICAL("Assimp: ", importer.GetErrorString());
            throw ResourceLoadingError;
        }

        const aiNode* root_node {scene->mRootNode};
        const aiMesh* mesh {find_mesh(root_node, object_name, scene)};

        if (mesh == nullptr) {
            LOG_CRITICAL("Model file `{}` does not contain mesh `{}`", file_path, object_name);
            throw ResourceLoadingError;
        }

        load(type, mesh, file_path);
    }

    Mesh::~Mesh() {
        delete[] vertices;
        delete[] indices;

        LOG_DEBUG("Freed model data");
    }

    void Mesh::load(Type type, const void* pmesh, const std::string& file_path) {
        const aiMesh* mesh {static_cast<const aiMesh*>(pmesh)};

        switch (type) {
            case Type::P: {
                std::vector<VertexP> vertices;
                std::vector<unsigned int> indices;

                load_P(mesh, vertices, indices);

                allocate(
                    vertices.data(),
                    vertices.size() * sizeof(VertexP),
                    indices.data(),
                    indices.size() * sizeof(unsigned int)
                );

                LOG_DEBUG("Loaded P model data `{}`", file_path);

                break;
            }
            case Type::PN: {
                std::vector<VertexPN> vertices;
                std::vector<unsigned int> indices;

                load_PN(mesh, vertices, indices);

                allocate(
                    vertices.data(),
                    vertices.size() * sizeof(VertexPN),
                    indices.data(),
                    indices.size() * sizeof(unsigned int)
                );

                LOG_DEBUG("Loaded PN model data `{}`", file_path);

                break;
            }
            case Type::PTN: {
                std::vector<VertexPTN> vertices;
                std::vector<unsigned int> indices;

                load_PTN(mesh, vertices, indices);

                allocate(
                    vertices.data(),
                    vertices.size() * sizeof(VertexPTN),
                    indices.data(),
                    indices.size() * sizeof(unsigned int)
                );

                LOG_DEBUG("Loaded PTN model data `{}`", file_path);

                break;
            }
            case Type::PTNT: {
                std::vector<VertexPTNT> vertices;
                std::vector<unsigned int> indices;

                load_PTNT(mesh, vertices, indices);

                allocate(
                    vertices.data(),
                    vertices.size() * sizeof(VertexPTNT),
                    indices.data(),
                    indices.size() * sizeof(unsigned int)
                );

                LOG_DEBUG("Loaded PTNT model data `{}`", file_path);

                break;
            }
        }
    }

    void Mesh::allocate(const void* vertices, std::size_t vertices_size, const void* indices, std::size_t indices_size) {
        this->vertices = new unsigned char[vertices_size];
        std::memcpy(this->vertices, vertices, vertices_size);
        this->vertices_size = vertices_size;

        this->indices = new unsigned char[indices_size];
        std::memcpy(this->indices, indices, indices_size);
        this->indices_size = indices_size;
    }
}
