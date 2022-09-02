#pragma once

#include <glad/glad.h>

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

    void bind();
    static void unbind();
private:
    GLuint buffer = 0;

    friend class VertexArray;
};

class UniformBuffer {
public:
    UniformBuffer();
    ~UniformBuffer();

    void bind();
    static void unbind();

    void set(const void* data, unsigned int field_index);
    void upload_data();
private:
    struct UniformBlockField {
        size_t offset, size;
    };

    GLuint buffer = 0;

    char* data = nullptr;  // Allocated externally, deallocated internally!
    size_t size = 0;

    std::unordered_map<unsigned int, UniformBlockField> fields;

    bool configured = false;  // Used externally by shader!

    friend class Shader;
};

class PixelBuffer {
public:
    PixelBuffer(size_t size);
    ~PixelBuffer();

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
    void* data = nullptr;
    char* dummy_data = nullptr;
};
