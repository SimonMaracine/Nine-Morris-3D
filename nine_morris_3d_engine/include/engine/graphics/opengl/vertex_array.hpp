#pragma once

#include <memory>

#include "engine/graphics/vertex_buffer_layout.hpp"

namespace sm {
    class GlVertexBuffer;
    class GlIndexBuffer;

    class GlVertexArray {
    public:
        class Def {
        public:
            Def& add_buffer(std::shared_ptr<GlVertexBuffer> buffer, const VertexBufferLayout& layout);
            Def& add_index_buffer(std::shared_ptr<GlIndexBuffer> index_buffer);
            void end_definition();
        private:
            Def() = default;

            friend class GlVertexArray;
        };

        GlVertexArray();
        ~GlVertexArray();

        GlVertexArray(const GlVertexArray&) = delete;
        GlVertexArray& operator=(const GlVertexArray&) = delete;
        GlVertexArray(GlVertexArray&&) = delete;
        GlVertexArray& operator=(GlVertexArray&&) = delete;

        void bind();
        static void unbind();

        Def begin_definition();
    private:
        unsigned int array = 0;
    };
}
