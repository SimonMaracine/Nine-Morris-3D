#pragma once

#include <string>
#include <memory>
#include <cstddef>

#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm {
    // Position, normals, texture coordinates, tangents
    enum class MeshType {
        P,
        PN,
        PNT,
        PNTT
    };

    static constexpr const char* MESH_DEFAULT_OBJECT {"defaultobject"};

    struct MeshSpecification {
        std::string object_name {MESH_DEFAULT_OBJECT};
        MeshType type {MeshType::P};
        bool generate_adjacency_indices {false};
        bool flip_winding {false};
    };

    // Resource representing a mesh with vertices and indices
    // It is used to create a vertex array
    class Mesh {
    public:
        Mesh(const std::string& buffer, const MeshSpecification& specification);
        ~Mesh() = default;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) = delete;
        Mesh& operator=(Mesh&&) = delete;

        const unsigned char* get_vertices() const;
        const unsigned char* get_indices() const;
        std::size_t get_vertices_size() const;
        std::size_t get_indices_size() const;
        const utils::AABB& get_aabb() const;
        MeshType get_type() const;
    private:
        void load(const MeshSpecification& specification, const void* pmesh);
        void allocate_vertices(const void* source, std::size_t size);
        void allocate_indices(const void* source, std::size_t size);

        // Raw data
        std::unique_ptr<unsigned char[]> m_vertices;
        std::unique_ptr<unsigned char[]> m_indices;
        std::size_t m_vertices_size {};
        std::size_t m_indices_size {};

        utils::AABB m_aabb;

        MeshType m_type;
    };
}
