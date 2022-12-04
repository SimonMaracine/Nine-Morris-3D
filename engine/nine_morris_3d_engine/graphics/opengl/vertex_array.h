#pragma once

#include <glad/glad.h>

#include "nine_morris_3d_engine/graphics/buffer_layout.h"
#include "nine_morris_3d_engine/graphics/opengl/buffer.h"

namespace gl {
    class VertexArray {
    public:
        VertexArray();
        ~VertexArray();

        void bind();
        static void unbind();

        void add_buffer(std::shared_ptr<Buffer> buffer, const BufferLayout& layout);
        void add_index_buffer(std::shared_ptr<IndexBuffer> index_buffer);
    private:
        GLuint array = 0;
    };
}
