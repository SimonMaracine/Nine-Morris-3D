#pragma once

#include <memory>
#include <vector>
#include <functional>

#include "nine_morris_3d_engine/graphics/opengl/buffer.hpp"
#include "nine_morris_3d_engine/graphics/opengl/vertex_buffer_layout.hpp"

namespace sm {
    // OpenGL resource representing a vertex array
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

        const GlIndexBuffer* get_index_buffer() const;
    private:
        unsigned int m_array {};

        std::vector<std::shared_ptr<GlVertexBuffer>> m_vertex_buffers;
        std::shared_ptr<GlIndexBuffer> m_index_buffer;
    };
}
