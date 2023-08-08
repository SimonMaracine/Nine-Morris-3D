#pragma once

#include <cstddef>

#include "engine/graphics/opengl/vertex_array.hpp"

namespace sm {
    enum class DrawHint {
        Static,
        Dynamic,
        Stream
    };

    class GlVertexBuffer {
    public:
        GlVertexBuffer(DrawHint hint = DrawHint::Static);
        GlVertexBuffer(size_t size, DrawHint hint = DrawHint::Static);
        GlVertexBuffer(const void* data, size_t size, DrawHint hint = DrawHint::Static);
        ~GlVertexBuffer();

        GlVertexBuffer(const GlVertexBuffer&) = delete;
        GlVertexBuffer& operator=(const GlVertexBuffer&) = delete;
        GlVertexBuffer(GlVertexBuffer&&) = delete;
        GlVertexBuffer& operator=(GlVertexBuffer&&) = delete;

        void bind();
        static void unbind();

        void upload_data(const void* data, size_t size);
        void upload_sub_data(const void* data, size_t offset, size_t size);
    private:
        unsigned int buffer = 0;
        DrawHint hint = DrawHint::Static;

        friend class VertexArray::Def;
    };

    class GlIndexBuffer {
    public:
        GlIndexBuffer(const unsigned int* data, size_t size);
        ~GlIndexBuffer();

        GlIndexBuffer(const GlIndexBuffer&) = delete;
        GlIndexBuffer& operator=(const GlIndexBuffer&) = delete;
        GlIndexBuffer(GlIndexBuffer&&) = delete;
        GlIndexBuffer& operator=(GlIndexBuffer&&) = delete;

        void bind();
        static void unbind();

        int get_index_count() { return index_count; }
    private:
        unsigned int buffer = 0;
        int index_count = 0;

        friend class VertexArray;
    };

    struct UniformBlockField {
        size_t offset = 0;
        size_t size = 0;
    };

    class GlUniformBuffer {
    public:
        GlUniformBuffer();
        ~GlUniformBuffer();

        GlUniformBuffer(const GlUniformBuffer&) = delete;
        GlUniformBuffer& operator=(const GlUniformBuffer&) = delete;
        GlUniformBuffer(GlUniformBuffer&&) = delete;
        GlUniformBuffer& operator=(GlUniformBuffer&&) = delete;

        void bind();
        static void unbind();

        bool is_configured() { return configured; }

        void set(const void* data, size_t field_index);
        void upload_sub_data();
    private:
        void allocate_memory(size_t size);
        void add_field(size_t index, const UniformBlockField& field);
        void configure();

        unsigned int buffer = 0;

        char* data = nullptr;
        size_t size = 0;

        std::unordered_map<size_t, UniformBlockField> fields;

        bool configured = false;

        friend class Shader;
    };

    class GlPixelBuffer {
    public:
        GlPixelBuffer(size_t size);
        ~GlPixelBuffer();

        GlPixelBuffer(const GlPixelBuffer&) = delete;
        GlPixelBuffer& operator=(const GlPixelBuffer&) = delete;
        GlPixelBuffer(GlPixelBuffer&&) = delete;
        GlPixelBuffer& operator=(GlPixelBuffer&&) = delete;

        void bind();
        static void unbind();

        void map_data();
        void unmap_data();

        template<typename T>
        void get_data(T** data_out) {
            *data_out = static_cast<T*>(data);
        }
    private:
        unsigned int buffer = 0;
        void* data = nullptr;  // TODO clear value is float; should be generic
        char* dummy_data = nullptr;
    };
}
