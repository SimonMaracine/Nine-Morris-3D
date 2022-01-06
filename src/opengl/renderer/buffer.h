#pragma once

#include <memory>
#include <cstddef>

#include <glad/glad.h>

class Buffer {
public:
    Buffer(GLuint buffer);
    ~Buffer();

    static std::shared_ptr<Buffer> create(size_t size);
    static std::shared_ptr<Buffer> create(const void* data, size_t size);

    void bind() const;
    static void unbind();

    void update_data(const void* data, size_t size) const;
private:
    GLuint buffer;

    friend class VertexArray;
    friend class Shader;
};

class IndexBuffer {
public:
    IndexBuffer(GLuint buffer);
    ~IndexBuffer();

    static std::shared_ptr<IndexBuffer> create(const unsigned int* data, size_t size);

    void bind() const;
    static void unbind();
private:
    GLuint buffer;

    friend class VertexArray;
    friend class Shader;
};

class UniformBuffer {
public:
    UniformBuffer(GLuint buffer);
    ~UniformBuffer();

    static std::shared_ptr<UniformBuffer> create(const void* data, size_t size);

    void bind() const;
    static void unbind();

    void update_data(const void* data, size_t size) const;
private:
    GLuint buffer;

    friend class VertexArray;
    friend class Shader;
};

template<typename T>
using Rc = std::shared_ptr<T>;
