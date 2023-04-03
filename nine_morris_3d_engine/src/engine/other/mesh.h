#pragma once

#include <glm/glm.hpp>

#include "engine/other/encrypt.h"

namespace mesh {
    struct Mesh {
    public:
        Mesh(const char* vertices, size_t vertices_size, const unsigned int* indices, size_t indices_size);
        ~Mesh();

        Mesh(const Mesh&) = delete;
        Mesh& operator=(const Mesh&) = delete;
        Mesh(Mesh&&) = delete;
        Mesh& operator=(Mesh&&) = delete;

        const char* get_vertices() { return vertices; }
        size_t get_vertices_size() { return vertices_size; }
        const unsigned int* get_indices() { return indices; }
        size_t get_indices_size() { return indices_size; }
    private:
        char* vertices = nullptr;
        size_t vertices_size = 0;
        unsigned int* indices = nullptr;
        size_t indices_size = 0;
    };

    std::shared_ptr<Mesh> load_model_PTN(std::string_view file_path, bool flip_winding = false);
    std::shared_ptr<Mesh> load_model_PTN(Encrypt::EncryptedFile file_path, bool flip_winding = false);

    std::shared_ptr<Mesh> load_model_P(std::string_view file_path, bool flip_winding = false);
    std::shared_ptr<Mesh> load_model_P(Encrypt::EncryptedFile file_path, bool flip_winding = false);

    std::shared_ptr<Mesh> load_model_PTNT(std::string_view file_path, bool flip_winding = false);
    std::shared_ptr<Mesh> load_model_PTNT(Encrypt::EncryptedFile file_path, bool flip_winding = false);
}
