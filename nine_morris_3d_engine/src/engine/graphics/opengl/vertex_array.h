#pragma once

#include <glad/glad.h>

#include "engine/graphics/buffer_layout.h"

namespace gl {
    class VertexBuffer;
    class IndexBuffer;
}

namespace gl {
    class VertexArray {
    public:
        class Def {
        public:
            Def& add_buffer(std::shared_ptr<VertexBuffer> buffer, const BufferLayout& layout);
            Def& add_index_buffer(std::shared_ptr<IndexBuffer> index_buffer);
            void end_definition();
        private:
            Def() = default;

            friend class VertexArray;
        };
    public:
        VertexArray();
        ~VertexArray();

        VertexArray(const VertexArray&) = delete;
        VertexArray& operator=(const VertexArray&) = delete;
        VertexArray(VertexArray&&) = delete;
        VertexArray& operator=(VertexArray&&) = delete;

        void bind();
        static void unbind();

        Def begin_definition();
    private:
        GLuint array = 0;
    };
}
