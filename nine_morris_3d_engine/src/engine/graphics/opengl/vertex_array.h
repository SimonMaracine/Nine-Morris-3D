#pragma once

#include <glad/glad.h>

#include "engine/graphics/buffer_layout.h"
#include "engine/graphics/opengl/buffer.h"

namespace gl {
    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;
        VertexArray(VertexArray&&) = delete;
        VertexArray& operator=(VertexArray&&) = delete;

        void bind();
        static void unbind();

        void add_buffer(std::shared_ptr<Buffer> buffer, const BufferLayout& layout);
        void add_index_buffer(std::shared_ptr<IndexBuffer> index_buffer);
    private:
        GLuint array = 0;
    };
}
