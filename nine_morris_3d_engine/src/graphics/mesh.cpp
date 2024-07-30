#include "nine_morris_3d_engine/graphics/mesh.hpp"

#include <vector>
#include <cstring>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "nine_morris_3d_engine/application/error.hpp"
#include "nine_morris_3d_engine/application/logging.hpp"

namespace sm {
    struct VertexP {
        aiVector3D position;
    };

    struct VertexPN {
        aiVector3D position;
        aiVector3D normal;
    };

    struct VertexPNT {
        aiVector3D position;
        aiVector3D normal;
        aiVector2D texture_coordinate;
    };

    struct VertexPNTT {
        aiVector3D position;
        aiVector3D normal;
        aiVector2D texture_coordinate;
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

    static void load_PNT(const aiMesh* mesh, std::vector<VertexPNT>& vertices, std::vector<unsigned int>& indices) {
        for (unsigned int i {0}; i < mesh->mNumVertices; i++) {
            VertexPNT vertex;
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

    static void load_PNTT(const aiMesh* mesh, std::vector<VertexPNTT>& vertices, std::vector<unsigned int>& indices) {
        for (unsigned int i {0}; i < mesh->mNumVertices; i++) {
            VertexPNTT vertex;
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
            const aiMesh* mesh {find_mesh(node->mChildren[i], object_name, scene)};

            if (mesh != nullptr) {
                return mesh;
            }
        }

        return nullptr;
    }

    Mesh::Mesh(const std::string& buffer, const std::string& object_name, Type type, bool flip_winding)
        : m_type(type) {
        unsigned int flags {aiProcess_ValidateDataStructure | aiProcess_GenBoundingBoxes};

        if (flip_winding) {
            flags |= aiProcess_FlipWindingOrder;
        }

        if (type == Type::PNTT) {
            flags |= aiProcess_CalcTangentSpace;
        }

        if (type != Type::P) {
            flags |= aiProcess_GenNormals;
        }

        Assimp::Importer importer;

        const aiScene* scene {importer.ReadFileFromMemory(buffer.data(), buffer.size(), flags)};

        if (scene == nullptr) {
            SM_THROW_ERROR(ResourceError, "Could not load model data: {}", importer.GetErrorString());
        }

        const aiNode* root_node {scene->mRootNode};
        const aiMesh* mesh {find_mesh(root_node, object_name, scene)};

        if (mesh == nullptr) {
            SM_THROW_ERROR(ResourceError, "Model file does not contain `{}` mesh", object_name);
        }

        load(type, mesh);

        m_aabb.min = glm::vec3(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z);
        m_aabb.max = glm::vec3(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z);
    }

    Mesh::~Mesh() noexcept {
        delete[] m_vertices;
        delete[] m_indices;

        LOG_DEBUG("Freed model data");
    }

    const unsigned char* Mesh::get_vertices() const noexcept {
        return m_vertices;
    }

    const unsigned char* Mesh::get_indices() const noexcept {
        return m_indices;
    }

    std::size_t Mesh::get_vertices_size() const noexcept {
        return m_vertices_size;
    }

    std::size_t Mesh::get_indices_size() const noexcept {
        return m_indices_size;
    }

    const utils::AABB& Mesh::get_aabb() const noexcept {
        return m_aabb;
    }

    Mesh::Type Mesh::get_type() const noexcept {
        return m_type;
    }

    void Mesh::load(Type type, const void* pmesh) {
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

                LOG_DEBUG("Loaded P model data");

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

                LOG_DEBUG("Loaded PN model data");

                break;
            }
            case Type::PNT: {
                std::vector<VertexPNT> vertices;
                std::vector<unsigned int> indices;

                load_PNT(mesh, vertices, indices);

                allocate(
                    vertices.data(),
                    vertices.size() * sizeof(VertexPNT),
                    indices.data(),
                    indices.size() * sizeof(unsigned int)
                );

                LOG_DEBUG("Loaded PNT model data");

                break;
            }
            case Type::PNTT: {
                std::vector<VertexPNTT> vertices;
                std::vector<unsigned int> indices;

                load_PNTT(mesh, vertices, indices);

                allocate(
                    vertices.data(),
                    vertices.size() * sizeof(VertexPNTT),
                    indices.data(),
                    indices.size() * sizeof(unsigned int)
                );

                LOG_DEBUG("Loaded PNTT model data");

                break;
            }
        }
    }

    void Mesh::allocate(const void* vertices, std::size_t vertices_size, const void* indices, std::size_t indices_size) {
        m_vertices = new unsigned char[vertices_size];
        std::memcpy(m_vertices, vertices, vertices_size);
        m_vertices_size = vertices_size;

        m_indices = new unsigned char[indices_size];
        std::memcpy(m_indices, indices, indices_size);
        m_indices_size = indices_size;
    }
}
