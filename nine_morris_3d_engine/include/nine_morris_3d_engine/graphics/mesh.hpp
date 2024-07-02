#pragma once

#include <cstddef>
#include <string>

#include "nine_morris_3d_engine/other/utilities.hpp"

namespace sm {
    class Mesh {
    public:
        enum class Type {
            P,
            PN,
            PNT,
            PNTT
        };

        static constexpr const char* DEFAULT_OBJECT {"defaultobject"};

        Mesh(const std::string& buffer, const std::string& object_name, Type type, bool flip_winding = false);
        ~Mesh();

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) = delete;
        Mesh& operator=(Mesh&&) = delete;

        const unsigned char* get_vertices() const { return vertices; }
        const unsigned char* get_indices() const { return indices; }
        std::size_t get_vertices_size() const { return vertices_size; }
        std::size_t get_indices_size() const { return indices_size; }
        const utils::AABB& get_aabb() const { return aabb; }
    private:
        void load(Type type, const void* pmesh);
        void allocate(const void* vertices, std::size_t vertices_size, const void* indices, std::size_t indices_size);

        // Raw data
        unsigned char* vertices {nullptr};
        unsigned char* indices {nullptr};
        std::size_t vertices_size {};
        std::size_t indices_size {};

        utils::AABB aabb;
    };
}
