#pragma once

#include <memory>
#include <cstddef>

#include <glad/glad.h>

class VertexBuffer {
public:
    enum class Type {
        Array, Index
    };

    VertexBuffer(GLuint buffer, Type type);
    ~VertexBuffer();

    static std::shared_ptr<VertexBuffer> create(std::size_t size);
    static std::shared_ptr<VertexBuffer> create_with_data(float* data, std::size_t size);
    static std::shared_ptr<VertexBuffer> create_index(unsigned int* data,
                                                      std::size_t size);

    void bind();
    static void unbind();  // This only unbinds the ARRAY_BUFFER, not ELEMENT_ARRAY_BUFFER
private:
    GLuint buffer;
    Type type;

    friend class VertexArray;
};
