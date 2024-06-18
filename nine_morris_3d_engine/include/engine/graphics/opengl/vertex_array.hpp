#pragma once

#include <memory>
#include <vector>
#include <functional>

#include "engine/graphics/opengl/buffer.hpp"
#include "engine/graphics/opengl/vertex_buffer_layout.hpp"

namespace sm {
    class GlVertexArray {
    public:
        using Configuration = std::function<void(GlVertexArray*)>;

        GlVertexArray();
        ~GlVertexArray();

        GlVertexArray(const GlVertexArray&) = delete;
        GlVertexArray& operator=(const GlVertexArray&) = delete;
        GlVertexArray(GlVertexArray&&) = delete;
        GlVertexArray& operator=(GlVertexArray&&) = delete;

        void bind() const;
        static void unbind();

        void configure(const Configuration& configuration);

        void add_vertex_buffer(std::shared_ptr<GlVertexBuffer> buffer, const VertexBufferLayout& layout);
        void add_index_buffer(std::shared_ptr<GlIndexBuffer> buffer);

        const GlIndexBuffer* get_index_buffer() const { return index_buffer.get(); }
    private:
        unsigned int array {};

        // Vertex arrays own vertex and index buffers
        std::vector<std::shared_ptr<GlVertexBuffer>> vertex_buffers;
        std::shared_ptr<GlIndexBuffer> index_buffer;
    };
}
