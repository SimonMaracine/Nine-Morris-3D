#pragma once

#include <string>
#include <memory>
#include <cstddef>

#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm {
    // Resource representing a mesh with vertices and indices
    // It is used to create a vertex array
    class Mesh {
    public:
        // Position, normals, texture coordinates, tangents
        enum class Type {
            P,
            PN,
            PNT,
            PNTT
        };

        static constexpr const char* DEFAULT_OBJECT {"defaultobject"};

        Mesh(const std::string& buffer, const std::string& object_name, Type type, bool flip_winding = false);
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
        Type get_type() const;
    private:
        void load(Type type, const void* pmesh);
        void allocate(const void* vertices, std::size_t vertices_size, const void* indices, std::size_t indices_size);

        // Raw data
        std::unique_ptr<unsigned char[]> m_vertices;
        std::unique_ptr<unsigned char[]> m_indices;
        std::size_t m_vertices_size {};
        std::size_t m_indices_size {};

        utils::AABB m_aabb;

        Type m_type;
    };
}
