#include <memory>
#include <cstddef>

#include <glad/glad.h>

#include "opengl/renderer/buffer.h"
#include "opengl/debug_opengl.h"
#include "other/logging.h"

Buffer::Buffer(GLuint buffer)
        : buffer(buffer) {
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

    return std::make_shared<Buffer>(buffer);
}

std::shared_ptr<Buffer> Buffer::create(const void* data, std::size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    LOG_ALLOCATION(size);

    return std::make_shared<Buffer>(buffer);
}

void Buffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
}

void Buffer::unbind() {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Buffer::update_data(const void* data, std::size_t size) const {
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

// --- Index buffer

IndexBuffer::IndexBuffer(GLuint buffer)
        : buffer(buffer) {
    SPDLOG_DEBUG("Created index buffer {}", buffer);
}

IndexBuffer::~IndexBuffer() {
    glDeleteBuffers(1, &buffer);

    SPDLOG_DEBUG("Deleted index buffer {}", buffer);
}

std::shared_ptr<IndexBuffer> IndexBuffer::create(const unsigned int* data, std::size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
    LOG_ALLOCATION(size);

    return std::make_shared<IndexBuffer>(buffer);
}

void IndexBuffer::bind() const {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer);
}

void IndexBuffer::unbind() {
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

// --- Uniform buffer

UniformBuffer::UniformBuffer(GLuint buffer)
        : buffer(buffer) {
    SPDLOG_DEBUG("Created uniform buffer {}", buffer);
}

UniformBuffer::~UniformBuffer() {
    glDeleteBuffers(1, &buffer);

    SPDLOG_DEBUG("Deleted uniform buffer {}", buffer);
}

std::shared_ptr<UniformBuffer> UniformBuffer::create(const void* data, std::size_t size) {
    GLuint buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_STATIC_DRAW);
    LOG_ALLOCATION(size);

    return std::make_shared<UniformBuffer>(buffer);
}

void UniformBuffer::bind() const {
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
}

void UniformBuffer::unbind() {
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void UniformBuffer::update_data(const void* data, std::size_t size) const {
    glBufferData(GL_UNIFORM_BUFFER, size, data, GL_STATIC_DRAW);
}
