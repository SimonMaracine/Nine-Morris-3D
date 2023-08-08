#pragma once

#include <memory>

#include "engine/graphics/vertex_buffer_layout.hpp"

namespace sm {
    class GlVertexBuffer;
    class GlIndexBuffer;

    class VertexArray {
    public:
        class Def {
        public:
            Def& add_buffer(std::shared_ptr<GlVertexBuffer> buffer, const VertexBufferLayout& layout);
            Def& add_index_buffer(std::shared_ptr<GlIndexBuffer> index_buffer);
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
        unsigned int array = 0;
    };
}
