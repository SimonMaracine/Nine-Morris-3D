#pragma once

#include <cstddef>
#include <string_view>
#include <memory>

#include <glm/glm.hpp>

#include "engine/other/encrypt.hpp"

namespace sm {
    class Mesh {
    public:
        Mesh(const void* vertices, std::size_t vertices_size, const void* indices, std::size_t indices_size);
        ~Mesh();

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) = delete;
        Mesh& operator=(Mesh&&) = delete;

        const char* get_vertices() const { return vertices; }
        const unsigned int* get_indices() const { return indices; }
        std::size_t get_vertices_size() const { return vertices_size; }
        std::size_t get_indices_size() const { return indices_size; }
    private:
        char* vertices = nullptr;
        unsigned int* indices = nullptr;
        std::size_t vertices_size = 0;
        std::size_t indices_size = 0;
    };

    struct Meshes {
        static std::shared_ptr<Mesh> load_model_PTN(std::string_view file_path, bool flip_winding = false);
        static std::shared_ptr<Mesh> load_model_PTN(Encrypt::EncryptedFile file_path, bool flip_winding = false);

        static std::shared_ptr<Mesh> load_model_P(std::string_view file_path, bool flip_winding = false);
        static std::shared_ptr<Mesh> load_model_P(Encrypt::EncryptedFile file_path, bool flip_winding = false);

        static std::shared_ptr<Mesh> load_model_PTNT(std::string_view file_path, bool flip_winding = false);
        static std::shared_ptr<Mesh> load_model_PTNT(Encrypt::EncryptedFile file_path, bool flip_winding = false);
    };
}
