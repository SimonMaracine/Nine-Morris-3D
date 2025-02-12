#pragma once

#include <memory>
#include <vector>
#include <functional>
#include <limits>

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

        // Configure this vertex array by binding it, calling the function and unbinding it
        void configure(const Configuration& configuration);

        // Bind and store a vertex buffer containing various attributes
        void add_vertex_buffer(std::shared_ptr<GlVertexBuffer> vertex_buffer, const VertexBufferLayout& layout);

        // Store an index buffer; this doesn't change any OpenGL state
        // You then access the buffers by their 0 based index
        void add_index_buffer(std::shared_ptr<GlIndexBuffer> index_buffer);

        // Bind an index buffer that was previously added
        void bind_index_buffer(std::size_t index);

        // Get an index buffer that was previously added
        std::shared_ptr<GlIndexBuffer> get_index_buffer(std::size_t index) const;
    private:
        static constexpr std::size_t INVALID_INDEX_BUFFER {std::numeric_limits<std::size_t>::max()};

        unsigned int m_array {};

        std::vector<std::shared_ptr<GlVertexBuffer>> m_vertex_buffers;
        std::vector<std::shared_ptr<GlIndexBuffer>> m_index_buffers;
        std::size_t m_index_buffer_index {INVALID_INDEX_BUFFER};
    };
}
