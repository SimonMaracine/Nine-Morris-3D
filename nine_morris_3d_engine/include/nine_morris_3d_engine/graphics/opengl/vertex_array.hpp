#pragma once

#include <memory>
#include <vector>
#include <functional>

#include "nine_morris_3d_engine/graphics/opengl/buffer.hpp"
#include "nine_morris_3d_engine/graphics/opengl/vertex_buffer_layout.hpp"

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

        void bind() const noexcept;
        static void unbind() noexcept;

        void configure(const Configuration& configuration);

        void add_vertex_buffer(std::shared_ptr<GlVertexBuffer> buffer, const VertexBufferLayout& layout);
        void add_index_buffer(std::shared_ptr<GlIndexBuffer> buffer);

        const GlIndexBuffer* get_index_buffer() const noexcept;
    private:
        unsigned int array {};

        std::vector<std::shared_ptr<GlVertexBuffer>> vertex_buffers;
        std::shared_ptr<GlIndexBuffer> index_buffer;
    };
}
