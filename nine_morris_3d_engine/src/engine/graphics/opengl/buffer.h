#pragma once

#include <glad/glad.h>

namespace gl {
    enum class DrawHint {
        Static = GL_STATIC_DRAW,
        Dynamic = GL_DYNAMIC_DRAW,
        Stream = GL_STREAM_DRAW
    };

    class Buffer {
    public:
        Buffer(size_t size, DrawHint hint = DrawHint::Static);
        Buffer(const void* data, size_t size, DrawHint hint = DrawHint::Static);
        ~Buffer();

        Buffer(const Buffer&) = delete;
        Buffer& operator=(const Buffer&) = delete;
        Buffer(Buffer&&) = delete;
        Buffer& operator=(Buffer&&) = delete;

        void bind();
        static void unbind();

        void update_data(const void* data, size_t size);
    private:
        GLuint buffer = 0;
        DrawHint hint = DrawHint::Static;

        friend class VertexArray;
    };

    class IndexBuffer {
    public:
        IndexBuffer(const unsigned int* data, size_t size);
        ~IndexBuffer();

        IndexBuffer(const IndexBuffer&) = delete;
        IndexBuffer& operator=(const IndexBuffer&) = delete;
        IndexBuffer(IndexBuffer&&) = delete;
        IndexBuffer& operator=(IndexBuffer&&) = delete;

        void bind();
        static void unbind();

        int get_index_count() { return index_count; }
    private:
        GLuint buffer = 0;
        int index_count = 0;

        friend class VertexArray;
    };

    class UniformBuffer {
    public:
        UniformBuffer();
        ~UniformBuffer();

        UniformBuffer(const UniformBuffer&) = delete;
        UniformBuffer& operator=(const UniformBuffer&) = delete;
        UniformBuffer(UniformBuffer&&) = delete;
        UniformBuffer& operator=(UniformBuffer&&) = delete;

        void bind();
        static void unbind();

        bool is_configured() { return configured; }

        void set(const void* data, size_t field_index);
        void upload_data();
    private:
        struct UniformBlockField {
            size_t offset, size;
        };

        GLuint buffer = 0;

        char* data = nullptr;  // Allocated externally, deallocated internally!
        size_t size = 0;

        std::unordered_map<size_t, UniformBlockField> fields;

        bool configured = false;  // Used externally by shader!

        friend class Shader;
    };

    class PixelBuffer {
    public:
        PixelBuffer(size_t size);
        ~PixelBuffer();

        PixelBuffer(const PixelBuffer&) = delete;
        PixelBuffer& operator=(const PixelBuffer&) = delete;
        PixelBuffer(PixelBuffer&&) = delete;
        PixelBuffer& operator=(PixelBuffer&&) = delete;

        void bind();
        static void unbind();

        void map_data();

        template<typename T>
        void get_data(T** data_out) {
            *data_out = static_cast<T*>(data);
        }

        void unmap_data();
    private:
        GLuint buffer = 0;
        void* data = nullptr;  // TODO clear value is float; should be generic
        char* dummy_data = nullptr;
    };
}
