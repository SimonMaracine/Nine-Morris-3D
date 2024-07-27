#pragma once

#include <string>
#include <cstddef>

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
        ~Mesh() noexcept;

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) = delete;
        Mesh& operator=(Mesh&&) = delete;

        const unsigned char* get_vertices() const noexcept;
        const unsigned char* get_indices() const noexcept;
        std::size_t get_vertices_size() const noexcept;
        std::size_t get_indices_size() const noexcept;
        const utils::AABB& get_aabb() const noexcept;
        Type get_type() const noexcept;
    private:
        void load(Type type, const void* pmesh);
        void allocate(const void* vertices, std::size_t vertices_size, const void* indices, std::size_t indices_size);

        // Raw data
        unsigned char* vertices {};
        unsigned char* indices {};
        std::size_t vertices_size {};
        std::size_t indices_size {};

        utils::AABB aabb;

        Type type;
    };
}
