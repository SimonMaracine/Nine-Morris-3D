#pragma once

#include <glm/glm.hpp>

#include "engine/other/encrypt.h"

namespace sm {
    namespace mesh {
        struct Mesh final {
        public:
            Mesh(const void* vertices, size_t vertices_size, const void* indices, size_t indices_size);
            ~Mesh();

            Mesh(const Mesh&) = delete;
            Mesh& operator=(const Mesh&) = delete;
            Mesh(Mesh&&) = delete;
            Mesh& operator=(Mesh&&) = delete;

            const char* get_vertices() const { return vertices; }
            const unsigned int* get_indices() const { return indices; }
            size_t get_vertices_size() const { return vertices_size; }
            size_t get_indices_size() const { return indices_size; }
        private:
            char* vertices = nullptr;
            unsigned int* indices = nullptr;
            size_t vertices_size = 0;
            size_t indices_size = 0;
        };

        std::shared_ptr<Mesh> load_model_PTN(std::string_view file_path, bool flip_winding = false);
        std::shared_ptr<Mesh> load_model_PTN(Encrypt::EncryptedFile file_path, bool flip_winding = false);

        std::shared_ptr<Mesh> load_model_P(std::string_view file_path, bool flip_winding = false);
        std::shared_ptr<Mesh> load_model_P(Encrypt::EncryptedFile file_path, bool flip_winding = false);

        std::shared_ptr<Mesh> load_model_PTNT(std::string_view file_path, bool flip_winding = false);
        std::shared_ptr<Mesh> load_model_PTNT(Encrypt::EncryptedFile file_path, bool flip_winding = false);
    }
}
