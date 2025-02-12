#include "nine_morris_3d_engine/graphics/mesh.hpp"

#include <vector>
#include <optional>
#include <cstring>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "nine_morris_3d_engine/application/internal/error.hpp"
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

    static std::optional<unsigned int> find_adjacent_index(const aiMesh* mesh, unsigned index1, unsigned int index2, unsigned int index3) {
        for (unsigned int i {0}; i < mesh->mNumFaces; i++) {
            const aiFace& face {mesh->mFaces[i]};

            for (int edge {0}; edge < 3; edge++) {
                unsigned int v1 {face.mIndices[edge]};  // First edge index
                unsigned int v2 {face.mIndices[(edge + 1) % 3]};  // Second edge index
                unsigned int vopp {face.mIndices[(edge + 2) % 3]};  // Opposite vertex index

                if ((v1 == index1 && v2 == index2 || v2 == index1 && v1 == index2) && vopp != index3) {
                    return vopp;
                }
            }
        }

        return std::nullopt;
    }

    static void push_mesh_indices(const aiMesh* mesh, std::vector<unsigned int>& indices) {
        for (unsigned int i {0}; i < mesh->mNumFaces; i++) {
            const aiFace& face {mesh->mFaces[i]};

            for (unsigned int j {0}; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);
            }
        }
    }

    static void push_mesh_adjacent_indices(const aiMesh* mesh, std::vector<unsigned int>& indices) {
        for (unsigned int i {0}; i < mesh->mNumFaces; i++) {
            const aiFace& face {mesh->mFaces[i]};

            for (unsigned int j {0}; j < face.mNumIndices; j++) {
                indices.push_back(face.mIndices[j]);

                unsigned int index1 {face.mIndices[j]};
                unsigned int index2 {face.mIndices[(j + 1) % face.mNumIndices]};
                unsigned int index3 {face.mIndices[(j + 2) % face.mNumIndices]};

                const auto vertex_adj {find_adjacent_index(mesh, index1, index2, index3)};

                if (!vertex_adj) {
                    SM_THROW_ERROR(internal::ResourceError, "Could not find adjacent vertex for mesh");
                }

                indices.push_back(*vertex_adj);
            }
        }
    }

    static void load_P(
        const aiMesh* mesh,
        std::vector<VertexP>& vertices,
        std::vector<unsigned int>& indices,
        bool generate_adjacency_indices
    ) {
        for (unsigned int i {0}; i < mesh->mNumVertices; i++) {
            VertexP vertex;
            vertex.position = mesh->mVertices[i];

            vertices.push_back(vertex);
        }

        if (generate_adjacency_indices) {
            push_mesh_adjacent_indices(mesh, indices);
        } else {
            push_mesh_indices(mesh, indices);
        }
    }

    static void load_PN(
        const aiMesh* mesh,
        std::vector<VertexPN>& vertices,
        std::vector<unsigned int>& indices,
        bool generate_adjacency_indices
    ) {
        for (unsigned int i {0}; i < mesh->mNumVertices; i++) {
            VertexPN vertex;
            vertex.position = mesh->mVertices[i];
            vertex.normal = mesh->mNormals[i];

            vertices.push_back(vertex);
        }

        if (generate_adjacency_indices) {
            push_mesh_adjacent_indices(mesh, indices);
        } else {
            push_mesh_indices(mesh, indices);
        }
    }

    static void load_PNT(
        const aiMesh* mesh,
        std::vector<VertexPNT>& vertices,
        std::vector<unsigned int>& indices,
        bool generate_adjacency_indices
    ) {
        for (unsigned int i {0}; i < mesh->mNumVertices; i++) {
            VertexPNT vertex;
            vertex.position = mesh->mVertices[i];
            vertex.normal = mesh->mNormals[i];
            vertex.texture_coordinate.x = mesh->mTextureCoords[0][i].x;
            vertex.texture_coordinate.y = mesh->mTextureCoords[0][i].y;

            vertices.push_back(vertex);
        }

        if (generate_adjacency_indices) {
            push_mesh_adjacent_indices(mesh, indices);
        } else {
            push_mesh_indices(mesh, indices);
        }
    }

    static void load_PNTT(
        const aiMesh* mesh,
        std::vector<VertexPNTT>& vertices,
        std::vector<unsigned int>& indices,
        bool generate_adjacency_indices
    ) {
        for (unsigned int i {0}; i < mesh->mNumVertices; i++) {
            VertexPNTT vertex;
            vertex.position = mesh->mVertices[i];
            vertex.normal = mesh->mNormals[i];
            vertex.texture_coordinate.x = mesh->mTextureCoords[0][i].x;
            vertex.texture_coordinate.y = mesh->mTextureCoords[0][i].y;
            vertex.tangent = mesh->mTangents[i];

            vertices.push_back(vertex);
        }

        if (generate_adjacency_indices) {
            push_mesh_adjacent_indices(mesh, indices);
        } else {
            push_mesh_indices(mesh, indices);
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

    Mesh::Mesh(const std::string& buffer, const MeshSpecification& specification)
        : m_type(specification.type) {
        auto flags {static_cast<unsigned int>(aiProcess_ValidateDataStructure | aiProcess_GenBoundingBoxes | aiProcess_JoinIdenticalVertices)};

        if (specification.flip_winding) {
            flags |= aiProcess_FlipWindingOrder;
        }

        if (specification.type != MeshType::P) {
            flags |= aiProcess_GenNormals;
        }

        if (specification.type == MeshType::PNTT) {
            flags |= aiProcess_CalcTangentSpace;
        }

        if (specification.generate_adjacency_indices) {
            assert(specification.type == MeshType::P);

            flags |= aiProcess_DropNormals;
            flags |= aiProcess_RemoveComponent;
        }

        Assimp::Importer importer;

        if (specification.generate_adjacency_indices) {
            importer.SetPropertyInteger(AI_CONFIG_PP_RVC_FLAGS, aiComponent_NORMALS | aiComponent_TEXCOORDS);
        }

        const aiScene* scene {importer.ReadFileFromMemory(buffer.data(), buffer.size(), flags)};

        if (scene == nullptr) {
            SM_THROW_ERROR(internal::ResourceError, "Could not load model data: {}", importer.GetErrorString());
        }

        const aiNode* root_node {scene->mRootNode};
        const aiMesh* mesh {find_mesh(root_node, specification.object_name, scene)};

        if (mesh == nullptr) {
            SM_THROW_ERROR(internal::ResourceError, "Model file does not contain `{}` mesh", specification.object_name);
        }

        load(specification, mesh);

        m_aabb.min = glm::vec3(mesh->mAABB.mMin.x, mesh->mAABB.mMin.y, mesh->mAABB.mMin.z);
        m_aabb.max = glm::vec3(mesh->mAABB.mMax.x, mesh->mAABB.mMax.y, mesh->mAABB.mMax.z);
    }

    const unsigned char* Mesh::get_vertices() const {
        return m_vertices.get();
    }

    const unsigned char* Mesh::get_indices() const {
        return m_indices.get();
    }

    std::size_t Mesh::get_vertices_size() const {
        return m_vertices_size;
    }

    std::size_t Mesh::get_indices_size() const {
        return m_indices_size;
    }

    const utils::AABB& Mesh::get_aabb() const {
        return m_aabb;
    }

    MeshType Mesh::get_type() const {
        return m_type;
    }

    void Mesh::load(const MeshSpecification& specification, const void* pmesh) {
        const aiMesh* mesh {static_cast<const aiMesh*>(pmesh)};

        std::vector<unsigned int> indices;

        switch (specification.type) {
            case MeshType::P: {
                std::vector<VertexP> vertices;
                load_P(mesh, vertices, indices, specification.generate_adjacency_indices);
                allocate_vertices(vertices.data(), vertices.size() * sizeof(VertexP));

                LOG_DEBUG("Loaded P model data");

                break;
            }
            case MeshType::PN: {
                std::vector<VertexPN> vertices;
                load_PN(mesh, vertices, indices, specification.generate_adjacency_indices);
                allocate_vertices(vertices.data(), vertices.size() * sizeof(VertexPN));

                LOG_DEBUG("Loaded PN model data");

                break;
            }
            case MeshType::PNT: {
                std::vector<VertexPNT> vertices;
                load_PNT(mesh, vertices, indices, specification.generate_adjacency_indices);
                allocate_vertices(vertices.data(), vertices.size() * sizeof(VertexPNT));

                LOG_DEBUG("Loaded PNT model data");

                break;
            }
            case MeshType::PNTT: {
                std::vector<VertexPNTT> vertices;
                load_PNTT(mesh, vertices, indices, specification.generate_adjacency_indices);
                allocate_vertices(vertices.data(), vertices.size() * sizeof(VertexPNTT));

                LOG_DEBUG("Loaded PNTT model data");

                break;
            }
        }

        allocate_indices(indices.data(), indices.size() * sizeof(unsigned int));
    }

    void Mesh::allocate_vertices(const void* source, std::size_t size) {
        m_vertices = std::make_unique<unsigned char[]>(size);
        std::memcpy(m_vertices.get(), source, size);
        m_vertices_size = size;
    }

    void Mesh::allocate_indices(const void* source, std::size_t size) {
        m_indices = std::make_unique<unsigned char[]>(size);
        std::memcpy(m_indices.get(), source, size);
        m_indices_size = size;
    }
}
