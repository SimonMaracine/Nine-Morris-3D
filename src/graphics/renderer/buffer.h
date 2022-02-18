#pragma once

#include <memory>
#include <cstddef>
#include <cassert>

#include <glad/glad.h>

class Buffer {
public:
    Buffer(GLuint buffer);
    ~Buffer();

    static std::shared_ptr<Buffer> create(size_t size);
    static std::shared_ptr<Buffer> create(const void* data, size_t size);

    void bind();
    static void unbind();

    void update_data(const void* data, size_t size);
private:
    GLuint buffer = 0;

    friend class VertexArray;
    friend class Shader;
};

class IndexBuffer {
public:
    IndexBuffer(GLuint buffer);
    ~IndexBuffer();

    static std::shared_ptr<IndexBuffer> create(const unsigned int* data, size_t size);

    void bind();
    static void unbind();
private:
    GLuint buffer = 0;

    friend class VertexArray;
    friend class Shader;
};

class UniformBuffer {
public:
    UniformBuffer(GLuint buffer);
    ~UniformBuffer();

    static std::shared_ptr<UniformBuffer> create(const void* data, size_t size);

    void bind();
    static void unbind();

    void update_data(const void* data, size_t size);
private:
    GLuint buffer = 0;

    friend class VertexArray;
    friend class Shader;
};

class PixelBuffer {
public:
    PixelBuffer(GLuint buffer, size_t size);
    ~PixelBuffer();

    static std::shared_ptr<PixelBuffer> create(size_t size);

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

    friend class VertexArray;
    friend class Shader;
};
