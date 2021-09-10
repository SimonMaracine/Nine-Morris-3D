#include <memory>
#include <cstddef>

#include <glad/glad.h>

#include "opengl/renderer/buffer.h"
#include "opengl/debug_opengl.h"
#include "other/logging.h"

Buffer::Buffer(GLuint buffer, Type type)
        : buffer(buffer), type(type) {
    SPDLOG_DEBUG("Created buffer {}", buffer);
}

Buffer::~Buffer() {
    glDeleteBuffers(1, &buffer);

    SPDLOG_DEBUG("Deleted buffer {}", buffer);
}

std::shared_ptr<Buffer> Buffer::create(std::size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, nullptr, GL_STATIC_DRAW);
    LOG_ALLOCATION(size);

    return std::make_shared<Buffer>(buffer, Type::Array);
}

std::shared_ptr<Buffer> Buffer::create(const void* data, std::size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    LOG_ALLOCATION(size);

    return std::make_shared<Buffer>(buffer, Type::Array);
}

std::shared_ptr<Buffer> Buffer::create_index(const unsigned int* data, std::size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    LOG_ALLOCATION(size);

    return std::make_shared<Buffer>(buffer, Type::Index);
}

std::shared_ptr<Buffer> Buffer::create_uniform(const void* data, std::size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
    LOG_ALLOCATION(size);

    return std::make_shared<Buffer>(buffer, Type::Uniform);
}

void Buffer::bind() const {
    switch (type) {
        case Type::Array: glBindBuffer(GL_ARRAY_BUFFER, buffer); break;
        case Type::Index: glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer); break;
        case Type::Uniform: glBindBuffer(GL_UNIFORM_BUFFER, buffer); break;
    }
}

void Buffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Buffer::update_data(const void* data, std::size_t size) const {
    switch (type) {
        case Type::Array: glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); break;
        case Type::Index: glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW); break;
        case Type::Uniform: glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW); break;
    }
}
