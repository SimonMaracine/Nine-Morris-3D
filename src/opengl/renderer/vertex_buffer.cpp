#include <memory>
#include <cstddef>

#include <glad/glad.h>

#include "opengl/renderer/vertex_buffer.h"
#include "logging.h"

VertexBuffer::VertexBuffer(GLuint buffer, Type type)
        : buffer(buffer), type(type) {
    SPDLOG_DEBUG("Created vertex buffer {}", buffer);
}

VertexBuffer::~VertexBuffer() {
    glDeleteBuffers(1, &buffer);

    SPDLOG_DEBUG("Deleted vertex buffer {}", buffer);
}

std::shared_ptr<VertexBuffer> VertexBuffer::create(std::size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);

    return std::make_shared<VertexBuffer>(buffer, Type::Array);
}

std::shared_ptr<VertexBuffer> VertexBuffer::create_with_data(float* data,
                                                             std::size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    return std::make_shared<VertexBuffer>(buffer, Type::Array);
}

std::shared_ptr<VertexBuffer> VertexBuffer::create_index(unsigned int* data,
                                                         std::size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);

    return std::make_shared<VertexBuffer>(buffer, Type::Index);
}

void VertexBuffer::bind() {
    switch (type) {
        case Type::Array: glBindBuffer(GL_ARRAY_BUFFER, buffer); break;
        case Type::Index: glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer); break;
    }
}

void VertexBuffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
